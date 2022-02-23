#pragma once

#include "tkFile/TkmFile.h"
#include "MeshParts.h"
#include "Skeleton.h"

namespace nsK2EngineLow {
	class IShaderResource;
	class ComputeAnimationVertexBuffer;


	/// <summary>
	/// モデルの初期化データ
	/// </summary>
	struct ModelInitData {
		const char* m_tkmFilePath = nullptr;							// tkmファイルパス。
		const char* m_vsEntryPointFunc = "VSMain";						// 頂点シェーダーのエントリーポイント。
		const char* m_vsSkinEntryPointFunc = "VSMain";					// スキンありマテリアル用の頂点シェーダーのエントリーポイント。
		const char* m_psEntryPointFunc = "PSMain";						// ピクセルシェーダーのエントリーポイント。
		const char* m_fxFilePath = nullptr;								// .fxファイルのファイルパス。
		void* m_expandConstantBuffer = nullptr;							// ユーザー拡張の定数バッファ。
		int m_expandConstantBufferSize = 0;								// ユーザー拡張の定数バッファのサイズ。
		std::array<IShaderResource*, MAX_MODEL_EXPAND_SRV> m_expandShaderResoruceView = { nullptr };	// ユーザー拡張のシェーダーリソース。
																										// t10レジスタから順番に割り当てられます。
		Skeleton* m_skeleton = nullptr;									// スケルトン。
		EnModelUpAxis m_modelUpAxis = enModelUpAxisZ;					// モデルの上方向。
		AlphaBlendMode m_alphaBlendMode = AlphaBlendMode_None;			// アルファブレンディングモード。
		bool m_isDepthWrite = true;										// 深度バッファに書き込む？
		bool m_isDepthTest = true;										// 深度テストを行う？
		D3D12_CULL_MODE m_cullMode = D3D12_CULL_MODE_BACK;				// カリングモード。
		ComputeAnimationVertexBuffer* m_computedAnimationVertexBuffer = nullptr;	// アニメーション済み頂点バッファを計算する処理。

		std::array<DXGI_FORMAT, MAX_RENDERING_TARGET> m_colorBufferFormat = {
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_UNKNOWN,
		};	//レンダリングするカラーバッファのフォーマット。
		
	};
	/// <summary>
	/// マテリアルを再初期化するためのデータ。
	/// </summary>
	struct MaterialReInitData {
		std::array<IShaderResource*, MAX_MODEL_EXPAND_SRV> m_expandShaderResoruceView = { nullptr };
	};
	/// <summary>
	/// モデルクラス。
	/// </summary>
	class Model : public Noncopyable {

	public:

		/// <summary>
		/// tkmファイルから初期化。
		/// </summary>
		/// <param name="initData">初期化データ</param>
		void Init(const ModelInitData& initData);
		/// <summary>
		/// ワールド行列の更新。
		/// </summary>
		/// <param name="pos">座標</param>
		/// <param name="rot">回転</param>
		/// <param name="scale">拡大率</param>
		void UpdateWorldMatrix(Vector3 pos, Quaternion rot, Vector3 scale)
		{
			m_worldMatrix = CalcWorldMatrix(pos, rot, scale);
		}

		/// <summary>
		/// ワールド行列を計算
		/// </summary>
		/// <remark>
		/// Modelクラスの設定に基づいたワールド行列の計算が行われます。
		/// 計算されたワールド行列が戻り値として返されます。
		/// 本関数はUpdateWorldMatrixから呼ばれています。
		/// 本関数はワールド行列を計算して、戻すだけです。
		/// Model::m_worldMatrixが更新されるわけではないので、注意してください。
		/// 本クラスの設定に基づいて計算されたワールド行列が必要な場合に使用してください
		/// </remark>
		/// <param name="pos">座標</param>
		/// <param name="rot">回転</param>
		/// <param name="scale">拡大率</param>
		/// <returns>ワールド行列</returns>
		Matrix CalcWorldMatrix(Vector3 pos, Quaternion rot, Vector3 scale);
		/// <summary>
		/// アニメーション済み頂点バッファの計算処理をディスパッチ。
		/// </summary>
		/// <param name="rc">レンダリングコンテキスト</param>
		void DispatchComputeAnimatedVertexBuffer(RenderContext& rc);
		/// <summary>
		/// 描画
		/// </summary>
		/// <param name="renderContext">レンダリングコンテキスト</param>
		/// <param name="numInstance">インスタンスの数</param>
		void Draw(
			RenderContext& renderContext,
			int numInstance = 1
		);
		/// <summary>
		/// 描画(カメラ指定版)
		/// </summary>
		/// <param name="renderContext">レンダリングコンテキスト</param>
		/// <param name="camera">カメラ</param>
		/// <param name="numInstance">インスタンスの数</param>
		void Draw(
			RenderContext& renderContext,
			Camera& camera,
			int numInstance = 1
		);
		/// <summary>
		/// 描画(カメラ行列指定版)
		/// </summary>
		/// <param name="renderContext">レンダリングコンテキスト</param>
		/// <param name="viewMatrix">ビュー行列</param>
		/// <param name="projMatrix">プロジェクション行列</param>
		/// <param name="numInstance">インスタンスの数</param>
		void Draw(
			RenderContext& renderContext,
			const Matrix& viewMatrix,
			const Matrix& projMatrix,
			int numInstance = 1
		);
		/// <summary>
		/// ワールド行列を取得。
		/// </summary>
		/// <returns></returns>
		const Matrix& GetWorldMatrix() const
		{
			return m_worldMatrix;
		}
		/// <summary>
		/// メッシュに対して問い合わせを行う。
		/// </summary>
		/// <param name="queryFunc">問い合わせ関数</param>
		void QueryMeshs(std::function<void(const SMesh& mesh)> queryFunc)
		{
			m_meshParts.QueryMeshs(queryFunc);
		}
		void QueryMeshAndDescriptorHeap(std::function<void(const SMesh& mesh, const DescriptorHeap& ds)> queryFunc)
		{
			m_meshParts.QueryMeshAndDescriptorHeap(queryFunc);
		}
		/// <summary>
		/// メッシュを取得。
		/// </summary>
		/// <param name="meshNo">メッシュ番号</param>
		/// <returns>メッシュ</returns>
		const SMesh& GetMesh(int meshNo) const
		{
			return m_meshParts.GetMesh(meshNo);
		}
		SMesh& GetMesh(int meshNo) 
		{
			return m_meshParts.GetMesh(meshNo);
		}
		/// <summary>
		/// アニメーション済み頂点バッファの計算処理を行っている？
		/// </summary>
		/// <returns></returns>
		bool IsComputedAnimationVertexBuffer() const
		{
			return m_meshParts.IsComputedAnimationVertexBuffer();
		}
		/// <summary>
		/// アニメーション計算済み頂点バッファを取得。
		/// </summary>
		/// <param name="meshNo">メッシュの番号</param>
		/// <returns>頂点バッファ</returns>
		const VertexBuffer& GetAnimatedVertexBuffer(int meshNo) const
		{
			return m_meshParts.GetAnimatedVertexBuffer(meshNo);
		}
		VertexBuffer& GetAnimatedVertexBuffer(int meshNo)
		{
			return m_meshParts.GetAnimatedVertexBuffer(meshNo);
		}
		/// <summary>
		/// 事前計算済みインデックスバッファを取得
		/// </summary>
		/// <param name="meshNo">メッシュ番号</param>
		/// <param name="matNo">マテリアル番号</param>
		/// <returns></returns>
		const IndexBuffer& GetAnimatedIndexBuffer(int meshNo, int matNo) const
		{
			return m_meshParts.GetAnimatedIndexBuffer(meshNo, matNo);
		}
		IndexBuffer& GetAnimatedIndexBuffer(int meshNo, int matNo)
		{
			return m_meshParts.GetAnimatedIndexBuffer(meshNo, matNo);
		}
		/// <summary>
		/// アルベドマップを変更。
		/// </summary>
		/// <remarks>
		/// この関数を呼び出すとディスクリプタヒープの再構築が行われるため、処理負荷がかかります。
		/// 毎フレーム呼び出す必要がない場合は呼び出さないようにしてください。
		/// </remarks>
		/// <param name="materialName">変更しいたマテリアルの名前</param>
		/// <param name="albedoMap">アルベドマップ</param>
		void ChangeAlbedoMap(const char* materialName, Texture& albedoMap);
		/// <summary>
		/// TKMファイルを取得。
		/// </summary>
		/// <returns></returns>
		const TkmFile& GetTkmFile() const
		{
			return *m_tkmFile;
		}
		/// <summary>
		/// 初期化されているか判定。
		/// </summary>
		/// <returns></returns>
		bool IsInited() const
		{
			return m_isInited;
		}
		/// <summary>
		/// マテリアルを再初期化。
		/// </summary>
		/// <remark>
		/// モデルに貼り付けるテクスチャを変更したい場合などに利用してください。
		/// </remark>
		/// <param name="reInitData">再初期化データ。</param>
		void ReInitMaterials(MaterialReInitData& reInitData);
	private:
		bool m_isInited = false;							// 初期化されている？
		Matrix m_worldMatrix;								// ワールド行列。
		TkmFile* m_tkmFile;									// tkmファイル。
		Skeleton m_skeleton;								// スケルトン。
		MeshParts m_meshParts;								// メッシュパーツ。
		EnModelUpAxis m_modelUpAxis = enModelUpAxisY;		// モデルの上方向。
		Model* m_computedAnimationVertexBuffer = nullptr;	// アニメーション後頂点バッファを計算済みのモデル。
	};
}
