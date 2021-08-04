#pragma once

#include "MyRenderer.h"
#include "geometry/AABB.h"

class RenderingEngine;


/// <summary>
/// スキンモデルレンダー。
/// </summary>
class ModelRender : public IRenderer 
{
public:
	ModelRender() {}
	/// <summary>
	/// 初期化。通常はこの関数で初期化してください。
	/// ディファードレンダリング。
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
	void Init(
		const char* filePath,
		AnimationClip* animationClips = nullptr,
		int numAnimationClips = 0,
		EnModelUpAxis enModelUpAxis = enModelUpAxisZ,
		bool isShadowReciever = true,
		int maxInstance = 1 );
	/// <summary>
	/// 初期化。
	/// フォワードレンダリング。
	/// この関数で初期化したモデルはライト、シャドウなどの影響を受けません。
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
	void InitForwardRendering(
		const char* filePath,
		AnimationClip* animationClips = nullptr,
		int numAnimationClips = 0,
		EnModelUpAxis enModelUpAxis = enModelUpAxisZ,
		bool isShadowReciever = true,
		int maxInstance = 1);
	/// <summary>
	/// Sky用。
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
	/// <param name="pos">座標</param>
	/// <param name="rot">回転</param>
	/// <param name="scale">拡大率</param>
	void UpdateInstancingData(const Vector3& pos, const Quaternion& rot, const Vector3& scale);
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
	void GetAABB(Vector3& vMax, Vector3& vMin, bool& isGet)
	{
		vMax = m_aabbMax;
		vMin = m_aabbMin;
		isGet = true;
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
private:

	/// <summary>
	/// シャドウマップ描画用のモデルを初期化。
	/// </summary>
	/// <param name="renderingEngine">レンダリングエンジン</param>
	/// <param name="tkmFilePath">tkmファイルパス</param>
	void InitModelOnShadowMap(
		RenderingEngine& renderingEngine,
		const char* tkmFilePath,
		EnModelUpAxis modelUpAxis
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
	/// GBuffer描画用のモデルを初期化。
	/// </summary>
	/// <param name="renderingEngine">レンダリングエンジン</param>
	/// <param name="tkmFilePath">tkmファイルパス</param>
	void InitModelOnRenderGBuffer(
		RenderingEngine& renderingEngine, 
		const char* tkmFilePath, 
		EnModelUpAxis enModelUpAxis,
		bool isShadowReciever );
	/// <summary>
	/// フォワードレンダリング用のモデルを初期化。
	/// </summary>
	/// <param name="renderingEngine"></param>
	/// <param name="tkmFilePath"></param>
	/// <param name="enModelUpAxis"></param>
	/// <param name="isShadowReciever"></param>
	void InitModelOnForward(
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
	/// ビューカリングを行う。
	/// </summary>
	/// <returns>trueが返ってくるとカリングされている。</returns>
	bool IsViewCulling(const Matrix& mWorld);
private:
		AnimationClip*				m_animationClips = nullptr;			// アニメーションクリップ。
	int							m_numAnimationClips = 0;			// アニメーションクリップの数。
	Vector3 					m_position = Vector3::Zero;			// 座標。
	Quaternion	 				m_rotation = Quaternion::Identity;	// 回転。
	Vector3						m_scale = Vector3::One;				// 拡大率。
	EnModelUpAxis				m_enFbxUpAxis = enModelUpAxisZ;		// FBXの上方向。
	Animation					m_animation;						// アニメーション。
	Model						m_zprepassModel;					// ZPrepassで描画されるモデル
	Model						m_forwardRenderModel;				// フォワードレンダリングの描画パスで描画されるモデル
	Model						m_renderToGBufferModel;				// RenderToGBufferで描画されるモデル
	Model						m_shadowModels[MAX_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP];	// シャドウマップに描画するモデル
	ConstantBuffer				m_drawShadowMapCameraParamCB[MAX_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP];		// シャドウマップ作成時に必要なカメラパラメータ用の定数バッファ。
	bool						m_isUpdateAnimation = true;			// アニメーションを更新する？
	Skeleton					m_skeleton;							// 骨。
	bool						m_isShadowCaster = true;			// シャドウキャスターフラグ
	float						m_animationSpeed = 1.0f;
	int							m_numInstance = 0;					// インスタンスの数。
	int							m_maxInstance = 1;					// 最大インスタンス数。
	int							m_fixNumInstanceOnFrame = 0;		// このフレームに描画するインスタンスの数の確定数。。
	bool						m_isEnableInstancingDraw = false;	// インスタンシング描画が有効？
	std::unique_ptr<Matrix[]>	m_worldMatrixArray;					// ワールド行列の配列。
	StructuredBuffer			m_worldMatrixArraySB;				// ワールド行列の配列のストラクチャードバッファ。
	AABB						m_aabb;								// モデルを内包するAABB
	bool						m_isViewCulling = false;			// ビューカリングされた？インスタンシング描画ではこのフラグは無効です。
	Vector3						m_aabbMax = {-FLT_MAX, -FLT_MAX, -FLT_MAX }; // AABBの最大値
	Vector3						m_aabbMin = { FLT_MAX, FLT_MAX, FLT_MAX };// AABBの最小値。
};

