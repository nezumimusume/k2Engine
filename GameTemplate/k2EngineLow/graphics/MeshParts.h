/// <summary>
/// メッシュパーツクラス。
/// </summary>

#pragma once

#include "tkFile/TkmFile.h"
#include "StructuredBuffer.h"

namespace nsK2EngineLow {

	class RenderContext;
	class Skeleton;
	class Material;
	class IShaderResource;
	class ComputeAnimationVertexBuffer;
	struct MaterialReInitData;


	const int MAX_MODEL_EXPAND_SRV = 32;	//拡張SRVの最大数。

	/// <summary>
	/// メッシュ
	/// </summary>
	struct SMesh {
		VertexBuffer m_vertexBuffer;							// 頂点バッファ。
		std::vector< IndexBuffer* >		m_indexBufferArray;		// インデックスバッファ。
		std::vector< Material* >		m_materials;			// マテリアル。
		std::vector<int>				skinFlags;				// スキンを持っているかどうかのフラグ。
	};

	/// <summary>
	/// メッシュパーツ。
	/// </summary>
	class MeshParts : public Noncopyable {
	public:
		/// <summary>
		/// デストラクタ。
		/// </summary>
		~MeshParts();
		
		/// <summary>
		/// tkmファイルから初期化
		/// </summary>
		/// <param name="tkmFile">tkmファイルのファイルパス</param>
		/// <param name="fxFilePath">シェーダーファイルのファイルパス</param>
		/// <param name="vsEntryPointFunc">スキンなし頂点シェーダーのエントリーポイント名</param>
		/// <param name="vsSkinEntryPointFunc">スキンあり頂点シェーダーのエントリーポイント名</param>
		/// <param name="psEntryPointFunc">ピクセルシェーダーのエントリーポイント名</param>
		/// <param name="expandData">拡張定数バッファ。b1にバインドされます。</param>
		/// <param name="expandDataSize">拡張定数バッファのサイズ</param>
		/// <param name="expandShaderResourceView">拡張シェーダーリソースビューの配列</param>
		/// <param name="colorBufferFormat">レンダリングターゲットのカラーバッファフォーマットの配列。</param>
		/// <param name="alphaBlendMode">アルファブレンディングモード</param>
		/// <param name="isDepthWrite">深度バッファに書き込むかどうかのフラグ</param>
		/// <param name="isDepthTest">深度テストを行うか同課のフラグ。</param>
		/// <param name="cullMode">カリングモード</param>
		/// <param name="computedAnimationVertexBuffer">アニメーション済み頂点バッファを計算する処理。</param>
		void InitFromTkmFile(
			const TkmFile& tkmFile,
			const char* fxFilePath,
			const char* vsEntryPointFunc,
			const char* vsSkinEntryPointFunc,
			const char* psEntryPointFunc,
			void* expandData,
			int expandDataSize,
			const std::array<IShaderResource*, MAX_MODEL_EXPAND_SRV>& expandShaderResourceView,
			const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat,
			AlphaBlendMode alphaBlendMode,
			bool isDepthWrite,
			bool isDepthTest,
			D3D12_CULL_MODE cullMode,
			ComputeAnimationVertexBuffer* computedAnimationVertexBuffer
		);
		/// <summary>
		/// 描画。
		/// </summary>
		/// <param name="rc">レンダリングコンテキスト</param>
		/// <param name="mWorld">ワールド行列</param>
		/// <param name="mView">ビュー行列</param>
		/// <param name="mProj">プロジェクション行列</param>
		/// <param name="numInstance">インスタンスの数</param>
		void Draw(
			RenderContext& rc,
			const Matrix& mWorld,
			const Matrix& mView,
			const Matrix& mProj,
			int numInstance);
		/// <summary>
		/// スケルトンを関連付ける。
		/// </summary>
		/// <param name="skeleton">スケルトン</param>
		void BindSkeleton(Skeleton& skeleton);
		/// <summary>
		/// アニメーション済み頂点バッファの計算処理を行っている？
		/// </summary>
		/// <returns></returns>
		bool IsComputedAnimationVertexBuffer() const
		{
			return m_computedAnimationVertexBuffer != nullptr;
		}
		/// <summary>
		/// アニメーション計算済み頂点バッファを取得。
		/// </summary>
		/// <param name="meshNo">メッシュの番号</param>
		/// <returns>頂点バッファ</returns>
		const VertexBuffer& GetAnimatedVertexBuffer(int meshNo) const;
		VertexBuffer& GetAnimatedVertexBuffer(int meshNo);
		/// <summary>
		/// 事前計算済みインデックスバッファを取得
		/// </summary>
		/// <param name="meshNo">メッシュ番号</param>
		/// <param name="matNo">マテリアル番号</param>
		/// <returns></returns>
		const IndexBuffer& GetAnimatedIndexBuffer(int meshNo, int matNo) const;
		IndexBuffer& GetAnimatedIndexBuffer(int meshNo, int matNo);
		/// <summary>
		/// メッシュに対して問い合わせを行う。
		/// </summary>
		/// <param name="queryFunc">クエリ関数</param>
		void QueryMeshs(std::function<void(const SMesh& mesh)> queryFunc)
		{
			for (const auto& mesh : m_meshs) {
				queryFunc(*mesh);
			}
		}
		void QueryMeshAndDescriptorHeap(std::function<void(const SMesh& mesh, const DescriptorHeap& ds)> queryFunc)
		{
			for (int i = 0; i < m_meshs.size(); i++) {
				queryFunc(*m_meshs[i], m_descriptorHeap);
			}
		}
		/// <summary>
		/// ディスクリプタヒープを作成。
		/// </summary>
		void CreateDescriptorHeaps();
		/// <summary>
		/// マテリアルを再初期化。
		/// </summary>
		void ReInitMaterials(const MaterialReInitData& reInitData);
		/// <summary>
		/// メッシュを取得。
		/// </summary>
		/// <param name="meshNo">メッシュ番号</param>
		/// <returns>メッシュ</returns>
		const SMesh& GetMesh(int meshNo) const
		{
			return *m_meshs.at(meshNo);
		}
		SMesh& GetMesh(int meshNo)
		{
			return *m_meshs.at(meshNo);
		}
	private:
		/// <summary>
		/// tkmメッシュからメッシュを作成。
		/// </summary>
		/// <param name="mesh">メッシュ</param>
		/// <param name="meshNo">メッシュ番号</param>
		/// <param name="fxFilePath">fxファイルのファイルパス</param>
		/// <param name="vsEntryPointFunc">頂点シェーダーのエントリーポイントの関数名</param>
		/// <param name="vsSkinEntryPointFunc">スキンありマテリアル用の頂点シェーダーのエントリーポイントの関数名</param>
		/// <param name="psEntryPointFunc">ピクセルシェーダーのエントリーポイントの関数名</param>
		/// <param name="colorBufferFormat">このモデルをレンダリングするカラーバッファのフォーマットの配列</param>
		/// <param name="alphaBlendMode">アルファブレンディングモード</param>
		/// <param name="isDepthWrite">深度値を深度バッファに書き込むかのフラグ</param>
		/// <param name="isDepthTest">深度テストを行うかどうかのフラグ</param>
		/// <param name="cullMode">カリングモード</param>
		/// <param name="computedAnimationVertexBuffer">アニメーション済み頂点バッファを計算する処理。</param>
		void CreateMeshFromTkmMesh(
			const TkmFile::SMesh& mesh,
			int meshNo,
			int& materialNum,
			const char* fxFilePath,
			const char* vsEntryPointFunc,
			const char* vsSkinEntryPointFunc,
			const char* psEntryPointFunc,
			const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat,
			AlphaBlendMode alphaBlendMode,
			bool isDepthWrite,
			bool isDepthTest,
			D3D12_CULL_MODE cullMode,
			ComputeAnimationVertexBuffer* computedAnimationVertexBuffer
		);


	private:
		//拡張SRVが設定されるレジスタの開始番号。
		const int EXPAND_SRV_REG__START_NO = 10;
		//１つのマテリアルで使用されるSRVの数。
		const int NUM_SRV_ONE_MATERIAL = EXPAND_SRV_REG__START_NO + MAX_MODEL_EXPAND_SRV;
		//１つのマテリアルで使用されるCBVの数。
		const int NUM_CBV_ONE_MATERIAL = 2;
		/// <summary>
		/// 定数バッファ。
		/// </summary>
		/// <remarks>
		/// この構造体を変更したら、SimpleModel.fxのCBも変更するように。
		/// </remarks>
		struct SConstantBuffer {
			Matrix mWorld;		//ワールド行列。
			Matrix mView;		//ビュー行列。
			Matrix mProj;		//プロジェクション行列。
		};
		ConstantBuffer m_commonConstantBuffer;					// メッシュ共通の定数バッファ。
		ConstantBuffer m_expandConstantBuffer;					// ユーザー拡張用の定数バッファ
		std::array<IShaderResource*, MAX_MODEL_EXPAND_SRV> m_expandShaderResourceView = { nullptr };	//ユーザー拡張シェーダーリソースビュー。
		StructuredBuffer m_boneMatricesStructureBuffer;			// ボーン行列の構造化バッファ。
		std::vector< SMesh* > m_meshs;							// メッシュ。
		//std::vector< DescriptorHeap > m_descriptorHeap;		// ディスクリプタヒープ。
		DescriptorHeap m_descriptorHeap;						// ディスクリプタヒープ。
		Skeleton* m_skeleton = nullptr;							// スケルトン。
		void* m_expandData = nullptr;							// ユーザー拡張データ。
		ComputeAnimationVertexBuffer* m_computedAnimationVertexBuffer = nullptr;	// アニメーション済み頂点バッファを計算する処理。
	};
}