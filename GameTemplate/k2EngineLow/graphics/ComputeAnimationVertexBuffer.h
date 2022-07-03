#pragma once


namespace nsK2EngineLow {
	/// <summary>
	/// アニメーション済み頂点バッファを計算する処理。
	/// </summary>
	class ComputeAnimationVertexBuffer : Noncopyable {
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="tkmFilePath">tkmファイルのファイルパス</param>
		/// <param name="numBone">ボーンの数</param>
		/// <param name="boneMatrixTopAddress">ボーン行列の配列</param>
		/// <param name="enModelUpAxis">モデルの上方向</param>
		/// <param name="numInstance">インスタンスの数</param>
		/// <param name="worldMatrixArraySB">
		/// ワールド行列の配列のストラクチャードバッファ。
		/// インスタンシングディスパッチを行う際は指定してください。
		/// </param>
		void Init(
			const char* tkmFilePath,
			int numBone,
			void* boneMatrixArray,
			EnModelUpAxis enModelUpAxis,
			int numInstance,
			StructuredBuffer* worldMatrixArraySB
		);
		
		/// <summary>
		/// ディスパッチ
		/// </summary>
		/// <param name="rc">レンダリングコンテキスト</param>
		/// <param name="worldMatrix">ワールド行列</param>
		/// <param name="instanceNo">インスタンス番号</param>
		void Dispatch(
			RenderContext& rc,
			const Matrix& worldMatrix,
			int instanceNo
		);
		
		/// <summary>
		/// アニメーション計算済み頂点バッファを取得。
		/// </summary>
		/// <param name="meshNo">メッシュの番号</param>
		/// <returns>頂点バッファ</returns>
		const VertexBuffer& GetAnimatedVertexBuffer(int meshNo) const
		{
			return m_meshArray[meshNo].m_animatedVertexBuffer;
		}
		VertexBuffer& GetAnimatedVertexBuffer(int meshNo)
		{
			return m_meshArray[meshNo].m_animatedVertexBuffer;
		}
		/// <summary>
		/// 事前計算済みインデックスバッファを取得。
		/// </summary>
		/// <param name="meshNo">メッシュ番号。</param>
		/// <param name="matNo">マテリアル番号</param>
		/// <returns>事前計算済みインデックスバッファ</returns>
		const IndexBuffer& GetAnimatedIndexBuffer(int meshNo, int matNo) const
		{
			return m_meshArray[meshNo].m_animatedIndexBufferArray[matNo];
		}
		IndexBuffer& GetAnimatedIndexBuffer(int meshNo, int matNo)
		{
			return m_meshArray[meshNo].m_animatedIndexBufferArray[matNo];
		}
	private:
		/// <summary>
		/// ディスクリプタヒープを初期化。
		/// </summary>
		void CreateDescriptorHeaps(StructuredBuffer* worldMatrixArraySB);
	private:
		/// <summary>
		/// b0レジスタにバインドされるデータ。
		/// </summary>
		struct CB_0 {
			Matrix worldMatrix;		// ワールド行列。
			int numVertex;			// 頂点数。
			int outputOffset = 0;	// 出力オフセット。
		};
		/// <summary>
		/// メッシュ単位の情報をひとまとめするための構造体。
		/// </summary>
		struct SMeshLocal {
			ConstantBuffer m_cb0;											// b0レジスタにバインドされる定数バッファ。
			RootSignature m_rootSignature;									// ルートシグネチャ。
			PipelineState m_pipelineState;									// パイプラインステート。
			VertexBuffer m_vertexBuffer;									// 頂点バッファ。
			VertexBuffer m_animatedVertexBuffer;							// アニメーション計算済みの頂点バッファ。
			StructuredBuffer m_vertexBufferSB;								// 頂点バッファのストラクチャードバッファ。
			RWStructuredBuffer m_animatedVertexBufferRWSB;					// アニメーション計算済みの頂点バッファのRWストラクチャードバッファ。
			std::unique_ptr< IndexBuffer[] > m_animatedIndexBufferArray;	// インデックスバッファの配列。
			std::unique_ptr< int[] > m_originalIndexBufferSize;				// オリジナルのインデックスバッファのサイズ。
			int m_numMaterial = 0;											// マテリアルの数。
		};
		bool m_isFirstDispatch = true;
		int m_numMesh = 0;									// メッシュの数。
		int m_numInstance = 0;								// インスタンスの数。
		void* m_boneMatrixArray = nullptr;					// ボーン行列の配列の先頭アドレス。
		DescriptorHeap m_descriptorHeap;					// ディスクリプタヒープ。
		Shader* m_shader;									// シェーダー。
		TkmFile m_tkmFile;									// tkmファイル。
		StructuredBuffer m_boneMatricesStructureBuffer;		// ボーン行列の構造化バッファ。
		std::unique_ptr< SMeshLocal[] > m_meshArray;		// メッシュ。
	};
}