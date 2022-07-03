#pragma once

#include "MyRenderer.h"
#include "geometry/AABB.h"
#include "geometry/geometryData.h"
#include "graphics/ComputeAnimationVertexBuffer.h"

namespace nsK2Engine {
	class RenderingEngine;

	/// <summary>
	/// スキンモデルレンダー。
	/// </summary>
	class ModelRender : public IRenderer
	{
	public:
		ModelRender();
		~ModelRender();
		/// <summary>
		/// 初期化。通常はこの関数で初期化してください。
		/// </summary>
		/// <param name="filePath">ファイルパス。</param>
		/// <param name="animationClips">アニメーションクリップ。</param>
		/// <param name="numAnimationClips">アニメーションクリップの数。</param>
		/// <param name="enModelUpAxis">モデルの上方向。</param>
		/// <param name="isShadowReciever">シャドウレシーバーフラグtrueなら影が落ちる。</param>
		/// <param name="maxInstance">
		/// インスタンスの最大数。この引数を１より大きくするとインスタンシング描画が行われます。
		/// インスタンシング描画を行う際は描画したいインスタンスの数分だけ、UpdateInstancingDraw()を呼び出す必要があります。
		/// インスタンシング描画の詳細はSmaple_XXを参照してください。
		/// </param>
		/// <param name="isFrontCullingOnDrawShadowMap">
		/// シャドウマップを描画する際にフロントカリングを行う？
		/// フロントカリングを行うとシャドウアクネが軽減できます。
		/// しかし、板ポリのような閉じていないモデルをフロントカリングで描画すると、
		/// シャドウマップに描画されなくなってしまうため、その場合はこのフラグをfalseにしてください。
		/// 例)地面など。
		/// </param>
		void Init(
			const char* filePath,
			AnimationClip* animationClips = nullptr,
			int numAnimationClips = 0,
			EnModelUpAxis enModelUpAxis = enModelUpAxisZ,
			bool isShadowReciever = true,
			int maxInstance = 1,
			bool isFrontCullingOnDrawShadowMap = false);
		/// <summary>
		/// 半透明描画を行うオブジェクトを初期化。
		/// </summary>
		/// <param name="filePath">ファイルパス。</param>
		/// <param name="animationClips">アニメーションクリップ。</param>
		/// <param name="numAnimationClips">アニメーションクリップの数。</param>
		/// <param name="enModelUpAxis">モデルの上方向。</param>
		/// <param name="isShadowReciever">trueなら影が落ちる。</param>
		/// <param name="maxInstance">
		/// インスタンスの最大数。この引数を１より大きくするとインスタンシング描画が行われます。
		/// インスタンシング描画を行う際は描画したいインスタンスの数分だけ、UpdateInstancingDraw()を呼び出す必要があります。
		/// インスタンシング描画の詳細はSmaple_XXを参照してください。
		/// </param>	
		/// <param name="isFrontCullingOnDrawShadowMap">
		/// シャドウマップを描画する際にフロントカリングを行う？
		/// フロントカリングを行うとシャドウアクネが軽減できます。
		/// しかし、板ポリのような閉じていないモデルをフロントカリングで描画すると、
		/// シャドウマップに描画されなくなってしまうため、その場合はこのフラグをfalseにしてください。
		/// 例)地面など。
		/// </param>
		void IniTranslucent(
			const char* filePath,
			AnimationClip* animationClips = nullptr,
			int numAnimationClips = 0,
			EnModelUpAxis enModelUpAxis = enModelUpAxisZ,
			bool isShadowReciever = true,
			int maxInstance = 1,
			bool isFrontCullingOnDrawShadowMap = false);
		/// <summary>
		/// 特殊なシェーディングを行いたい場合の初期化処理。
		/// </summary>
		/// <param name="initData">モデルデータ。</param>
		void InitForwardRendering(ModelInitData& initData);
		/// <summary>
		/// 更新処理。
		/// </summary>
		void Update();
		/// <summary>
		/// インスタンシングデータの更新。
		/// </summary>
		/// <param name="instanceNo">インスタンス番号</param>
		/// <param name="pos">座標</param>
		/// <param name="rot">回転</param>
		/// <param name="scale">拡大率</param>
		void UpdateInstancingData(int instanceNo, const Vector3& pos, const Quaternion& rot, const Vector3& scale);
		/// <summary>
		/// 描画処理。
		/// </summary>
		void Draw(RenderContext& rc);

		/// <summary>
		/// アニメーション再生。
		/// </summary>
		/// <param name="animNo">アニメーションクリップの番号。</param>
		/// <param name="interpolateTime">補完時間(単位：秒。)</param>
		void PlayAnimation(int animNo, float interpolateTime = 0.0f)
		{
			m_animation.Play(animNo, interpolateTime);
		}

		/// <summary>
		/// アニメーションの再生中？
		/// </summary>
		bool IsPlayingAnimation() const
		{
			return m_animation.IsPlaying();
		}

		/// <summary>
		/// モデルを取得。
		/// </summary>
		/// <returns>モデル</returns>
		Model& GetModel()
		{
			if (m_renderToGBufferModel.IsInited()) {
				return m_renderToGBufferModel;
			}
			else if (m_forwardRenderModel.IsInited()) {
				return m_forwardRenderModel;
			}
			else if (m_translucentModel.IsInited())
			{
				return m_translucentModel;
			}
			// ここまで来るのはおかしい。
			return m_zprepassModel;
		}

		/// <summary>
		/// 座標、回転、拡大を全て設定。
		/// </summary>
		/// <remark>
		/// インスタンシング描画が有効の場合は、この設定は無視されます。
		/// </remark>
		/// <param name="pos">座標。</param>
		/// <param name="rotation">回転。</param>
		/// <param name="scale">拡大。</param>
		void SetTRS(const Vector3& pos, const Quaternion& rotation, const Vector3& scale)
		{
			SetPosition(pos);
			SetRotation(rotation);
			SetScale(scale);
		}

		/// <summary>
		/// 座標を設定。
		/// </summary>
		/// <remark>
		/// インスタンシング描画が有効の場合は、この設定は無視されます。
		/// </remark>
		/// <param name="pos">座標。</param>
		void SetPosition(const Vector3& pos)
		{
			m_position = pos;
		}
		/// <summary>
		/// 座標を設定。
		/// </summary>
		/// <param name="x">x座標</param>
		/// <param name="y">y座標</param>
		/// <param name="z">z座標</param>
		void SetPosition(float x, float y, float z)
		{
			SetPosition({ x, y, z });
		}
		/// <summary>
		/// 回転を設定。
		/// </summary>
		/// <remark>
		/// インスタンシング描画が有効の場合は、この設定は無視されます。
		/// </remark>
		/// <param name="rotation">回転。</param>
		void SetRotation(const Quaternion& rotation)
		{
			m_rotation = rotation;
		}

		/// <summary>
		/// 拡大率を設定。
		/// </summary>
		/// <remark>
		/// インスタンシング描画が有効の場合は、この設定は無視されます。
		/// </remark>
		/// <param name="scale">拡大率。</param>
		void SetScale(const Vector3& scale)
		{
			m_scale = scale;
		}
		void SetScale(float x, float y, float z)
		{
			SetScale({ x, y, z });
		}
		/// <summary>
		/// シャドウキャスターのフラグを設定する
		/// </summary>
		void SetShadowCasterFlag(bool flag)
		{
			m_isShadowCaster = flag;
		}
		/// <summary>
		/// アニメーション再生の速度を設定する。
		/// </summary>
		/// <param name="animationSpeed">数値の分だけ倍にする。</param>
		void SetAnimationSpeed(const float animationSpeed)
		{
			m_animationSpeed = animationSpeed;
		}
		/// <summary>
		/// ボーンの名前からボーン番号を検索。
		/// </summary>
		/// <param name="boneName">ボーンの名前</param>
		/// <returns>ボーン番号。見つからなかった場合は-1が返ってきます。</returns>
		int FindBoneID(const wchar_t* boneName) const
		{
			return m_skeleton.FindBoneID(boneName);
		}
		/// <summary>
		/// ボーン番号からボーンを取得。
		/// </summary>
		/// <param name="boneNo">ボーン番号</param>
		/// <returns>ボーン</returns>
		Bone* GetBone(int boneNo) const
		{
			return m_skeleton.GetBone(boneNo);
		}
		void AddAnimationEvent(AnimationEventListener eventListener)
		{
			m_animation.AddAnimationEventListener(eventListener);
		}

		/// <summary>
		/// シャドウキャスター？
		/// </summary>
		/// <returns></returns>
		bool IsShadowCaster() const
		{
			return m_isShadowCaster;
		}
		/// <summary>
		/// インスタン数を取得。
		/// </summary>
		/// <returns></returns>
		int GetNumInstance() const
		{
			return m_numInstance;
		}
		/// <summary>
		/// インスタンシング描画を行う？
		/// </summary>
		/// <returns></returns>
		bool IsInstancingDraw() const
		{
			return m_isEnableInstancingDraw;
		}
		/// <summary>
		/// ワールド行列を取得。
		/// </summary>
		/// <param name="instanceId">
		/// インスタンスID。
		/// この引数はインスタンシング描画が向こうの場合は無視されます。
		/// </param>
		/// <returns></returns>
		const Matrix& GetWorldMatrix(int instanceId) const
		{
			if (IsInstancingDraw()) {
				return m_worldMatrixArray[instanceId];
			}
			return m_zprepassModel.GetWorldMatrix();
		}
		/// <summary>
		/// インスタンスを除去。
		/// </summary>
		/// <remark>
		/// インスタンス描画を利用している際に、そのインスタンスをシーンから除去したい場合に利用してください。
		/// </remark>
		/// <param name="instanceNo">インスタンス番号</param>
		void RemoveInstance(int instanceNo);
		/// <summary>
		/// レイトレワールドに登録するかのフラグを設定する
		/// 頻繁に生成するモデル（弾など）をInitする前に引数falseで実行してください。
		/// </summary>
		void SetRaytracingWorld(bool flag) {
			m_isRaytracingWorld = flag;
		}
	private:
		/// <summary>
		/// スケルトンの初期化。
		/// </summary>
		/// <param name="filePath">ファイルパス。</param>
		void InitSkeleton(const char* filePath);
		/// <summary>
		/// アニメーションの初期化。
		/// </summary>
		/// <param name="animationClips">アニメーションクリップ。</param>
		/// <param name="numAnimationClips">アニメーションクリップの数。</param>
		/// <param name="enModelUpAxis">モデルの上向き。</param>
		void InitAnimation(AnimationClip* animationClips,
			int numAnimationClips,
			EnModelUpAxis enModelUpAxis);
		/// <summary>
		/// 頂点計算パスから呼び出される処理。
		/// </summary>
		/// <param name="rc"></param>
		void OnComputeVertex(RenderContext& rc);
		/// <summary>
		/// シャドウマップへの描画パスから呼ばれる処理。
		/// </summary>
		/// <param name="rc">レンダリングコンテキスト</param>
		/// <param name="ligNo">ライト番号</param>
		/// <param name="shadowMapNo">シャドウマップ番号</param>
		/// <param name="lvpMatrix">ライトビュープロジェクション行列</param>
		void OnRenderShadowMap(
			RenderContext& rc,
			int ligNo,
			int shadowMapNo,
			const Matrix& lvpMatrix) override;
		/// <summary>
		/// ZPrepassから呼ばれる処理。
		/// </summary>
		/// <param name="rc"></param>
		void OnZPrepass(RenderContext& rc) override;
		/// <summary>
		/// G-Buffer描画パスから呼ばれる処理。
		/// </summary>
		void OnRenderToGBuffer(RenderContext& rc) override;
		/// <summary>
		/// フォワードレンダーパスから呼ばれる処理。
		/// </summary>
		void OnForwardRender(RenderContext& rc) override;
		/// <summary>
		/// 半透明オブジェクト描画パスから呼ばれる処理。
		/// </summary>
		/// <param name="rc"></param>
		void OnTlanslucentRender(RenderContext& rc) override;
		/// <summary>
		/// 各種モデルのワールド行列を更新する。
		/// </summary>
		void UpdateWorldMatrixInModes();
	private:
		/// <summary>
		/// レンダリングエンジンで発生したイベントを受け取ったときに呼ばれる処理。
		/// </summary>
		/// <param name="enEvent"></param>
		void OnRecievedEventFromRenderingEngine(RenderingEngine::EnEvent enEvent);
		/// <summary>
		/// シャドウマップ描画用のモデルを初期化。
		/// </summary>
		/// <param name="renderingEngine">レンダリングエンジン</param>
		/// <param name="tkmFilePath">tkmファイルパス</param>
		/// <param name="modelUpAxis">モデルの上軸</param>
		void InitModelOnShadowMap(
			RenderingEngine& renderingEngine,
			const char* tkmFilePath,
			EnModelUpAxis modelUpAxis,
			bool isFrontCullingOnDrawShadowMap
		);
		/// <summary>
		/// ZPrepass描画用のモデルを初期化。
		/// </summary>
		/// <param name="renderingEngine"></param>
		/// <param name="tkmFilePath"></param>
		void InitModelOnZprepass(
			RenderingEngine& renderingEngine,
			const char* tkmFilePath,
			EnModelUpAxis modelUpAxis
		);

		/// <summary>
		/// インスタンシング描画用の初期化処理を実行。
		/// </summary>
		/// <param name="maxInstance">最大インスタンス数</param>
		void InitInstancingDraw(int maxInstance);
		/// <summary>
		/// アニメーション済み頂点バッファの計算処理を初期化。
		/// </summary>
		/// <param name="tkmFilePath">tkmファイルのファイルパス</param>
		/// <param name="enModelUpAxis">モデルの上軸</param>
		void InitComputeAnimatoinVertexBuffer(
			const char* tkmFilePath,
			EnModelUpAxis enModelUpAxis);
		/// <summary>
		/// GBuffer描画用のモデルを初期化。
		/// </summary>
		/// <param name="renderingEngine">レンダリングエンジン</param>
		/// <param name="tkmFilePath">tkmファイルパス</param>
		void InitModelOnRenderGBuffer(
			RenderingEngine& renderingEngine,
			const char* tkmFilePath,
			EnModelUpAxis enModelUpAxis,
			bool isShadowReciever);
		/// <summary>
		/// 半透明オブジェクト描画パスで使用されるモデルを初期化。
		/// </summary>
		/// <param name="renderingEngine"></param>
		/// <param name="tkmFilePath"></param>
		/// <param name="enModelUpAxis"></param>
		/// <param name="isShadowReciever"></param>
		void InitModelOnTranslucent(
			RenderingEngine& renderingEngine,
			const char* tkmFilePath,
			EnModelUpAxis enModelUpAxis,
			bool isShadowReciever
		);
		/// <summary>
		/// 各種モデルの頂点シェーダーのエントリーポイントを設定。
		/// </summary>
		void SetupVertexShaderEntryPointFunc(ModelInitData& modelInitData);
		/// <summary>
		/// モデルを内包するバウンディングボリュームを初期化。
		/// </summary>
		/// <remark>
		/// AABBを構築します。
		/// </remark>
		void InitBoundingVolume();
		/// <summary>
		/// 幾何学情報を初期化
		/// </summary>
		/// <param name="maxInstance">インスタンス数</param>
		void InitGeometryDatas(int maxInstance);
	public:
		static const int NUM_SHADOW_LIGHT = 1;
	private:
		AnimationClip* m_animationClips = nullptr;			// アニメーションクリップ。
		int							m_numAnimationClips = 0;			// アニメーションクリップの数。
		Vector3 					m_position = Vector3::Zero;			// 座標。
		Quaternion	 				m_rotation = Quaternion::Identity;	// 回転。
		Vector3						m_scale = Vector3::One;				// 拡大率。
		EnModelUpAxis				m_enFbxUpAxis = enModelUpAxisZ;		// FBXの上方向。
		Animation					m_animation;						// アニメーション。
		ComputeAnimationVertexBuffer m_computeAnimationVertexBuffer;	// アニメーション済み頂点バッファの計算処理。
		Model*						m_addRaytracingWorldModel = nullptr;// レイトレワールドに登録したモデル。				
		Model						m_zprepassModel;					// ZPrepassで描画されるモデル
		Model						m_forwardRenderModel;				// フォワードレンダリングの描画パスで描画されるモデル
		Model						m_translucentModel;					// 半透明モデル。
		Model						m_renderToGBufferModel;				// RenderToGBufferで描画されるモデル
		Model						m_shadowModels[MAX_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP];	// シャドウマップに描画するモデル
		ConstantBuffer				m_drawShadowMapCameraParamCB[MAX_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP];		// シャドウマップ作成時に必要なカメラパラメータ用の定数バッファ。
		bool						m_isUpdateAnimation = true;			// アニメーションを更新する？
		Skeleton					m_skeleton;							// 骨。
		bool						m_isShadowCaster = true;			// シャドウキャスターフラグ
		float						m_animationSpeed = 1.0f;
		int							m_numInstance = 0;					// インスタンスの数。
		int							m_maxInstance = 1;					// 最大インスタンス数。
		bool						m_isEnableInstancingDraw = false;	// インスタンシング描画が有効？
		bool						m_isRaytracingWorld = true;			//レイトレワールドに登録する？
		std::unique_ptr<Matrix[]>	m_worldMatrixArray;					// ワールド行列の配列。
		StructuredBuffer			m_worldMatrixArraySB;				// ワールド行列の配列のストラクチャードバッファ。
		std::vector< GemometryData > m_geometryDatas;					// ジオメトリ情報。
		std::unique_ptr<int[]>		m_instanceNoToWorldMatrixArrayIndexTable;	// インスタンス番号からワールド行列の配列のインデックスに変換するテーブル。

	};
}
