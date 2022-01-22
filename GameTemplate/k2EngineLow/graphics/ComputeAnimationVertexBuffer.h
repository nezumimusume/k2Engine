#pragma once


namespace nsK2EngineLow {
	/// <summary>
	/// アニメーション済み頂点バッファを計算する処理。
	/// </summary>
	class ComputeAnimationVertexBuffer : Noncopyable{
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="tkmFilePath">tkmファイルのファイルパス</param>
		/// <param name="enModelUpAxis">モデルの上方向</param>
		void Init(const char* tkmFilePath, EnModelUpAxis enModelUpAxis);
		/// <summary>
		/// ディスパッチ
		/// </summary>
		/// <param name="rc">レンダリングコンテキスト</param>
		void Dispatch(RenderContext& rc);
		/// <summary>
		/// アニメーション計算済み頂点バッファを取得。
		/// </summary>
		/// <param name="meshNo">メッシュの番号</param>
		/// <returns>頂点バッファ</returns>
		const VertexBuffer& GetAnimatedVertexBuffer(int meshNo ) const
		{
			return m_animatedVertexBufferArray[meshNo];
		}
		VertexBuffer& GetAnimatedVertexBuffer(int meshNo) 
		{
			return m_animatedVertexBufferArray[meshNo];
		}		
	private:
		/// <summary>
		/// ディスクリプタヒープを初期化。
		/// </summary>
		void CreateDescriptorHeaps();
	private:
		int m_numMesh = 0;														// メッシュの数。
		DescriptorHeap m_descriptorHeap;										// ディスクリプタヒープ。
		Shader m_shader;														// シェーダー。
		TkmFile m_tkmFile;														// tkmファイル。
		std::unique_ptr< PipelineState[]> m_pipilineStateArray;					// パイプラインステートの配列。
		std::unique_ptr<RootSignature[]> m_rootSignatureArray;					// ルートシグネチャの配列。
		std::unique_ptr<VertexBuffer[]> m_vertexBufferArray;					// 頂点バッファの配列。配列のインデックスはtkmファイルのメッシュ番号に対応しています。
		std::unique_ptr<VertexBuffer[]> m_animatedVertexBufferArray;			// アニメーション計算済み頂点バッファの配列。配列のインデックスはtkmファイルのメッシュ番号に対応しています。
		std::unique_ptr<StructuredBuffer[]> m_vertexBufferSBArray;				// 頂点バッファのストラクチャードバッファの配列。
		std::unique_ptr<RWStructuredBuffer[]> m_animatedVertexBufferRWSBArray;	// アニメーション計算済み頂点バッファのRWストラクチャードバッファ。配列のインデックスはtkmファイルのメッシュ番号に対応しています。
	};
}