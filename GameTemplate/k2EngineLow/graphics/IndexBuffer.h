#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// インデックスバッファ。
	/// </summary>
	/// <remarks>
	/// </remarks>
	class IndexBuffer : public Noncopyable {
	public:
		/// <summary>
		/// デストラクタ。
		/// </summary>
		~IndexBuffer();
		/// <summary>
		/// 初期化。
		/// </summary>
		/// <param name="size">インデックスバッファのサイズ。</param>
		/// <param name="stride">ストライド。</param>
		void Init(int size, int stride);
		/// <summary>
		/// インデックスデータをインデックスバッファにコピー。
		/// </summary>
		/// <param name="srcIndecies">コピー元のインデックスデータ。</param>
		/// <param name="numCopy">コピーする数。０を指定するとコピー先のインデックスの数と同じだけコピーされます。</param>
		/// <param name="copyStartAddrOffset">コピー開始アドレスのオフセット</param>
		/// <param name="srcIndexBias">コピー元インデックスを底上げする値。</param>
		/// <remark>
		/// ほとんどの場合で、第２引数～第４引数は0を指定してください。
		/// 部分的なコピーを行いたい場合はコピーする数を指定する、
		/// numCopyやコピー先のアドレスをオフセットするcopyStartAddrOffsetに値を設定してください。
		/// また、コピー元のインデックスの値を一律で底上げを行いたい場合はsrcIndexBiasに値を設定してください。
		/// 例えば、srcIndexBiasに10が指定されていると、srcIndeciesに記憶されている値に+10加算したものがコピーされていきます。
		/// </remark>
		void Copy(uint16_t* srcIndecies, int numCopy, uint32_t copyStartAddrOffset, uint32_t srcIndexBias);

		/// <summary>
		/// インデックスデータをインデックスバッファにコピー。
		/// </summary>
		/// <param name="srcIndecies">コピー元のインデックスデータ。</param>
		/// <param name="numCopy">コピーする数。０を指定するとコピー先のインデックスの数と同じだけコピーされます。</param>
		/// <param name="copyStartAddrOffset">コピー開始アドレスのオフセット</param>
		/// <param name="srcIndexBias">コピー元インデックスを底上げする値。</param>
		/// <remark>
		/// ほとんどの場合で、第２引数～第４引数は0を指定してください。
		/// 部分的なコピーを行いたい場合はコピーする数を指定する、
		/// numCopyやコピー先のアドレスをオフセットするcopyStartAddrOffsetに値を設定してください。
		/// また、コピー元のインデックスの値を一律で底上げを行いたい場合はsrcIndexBiasに値を設定してください。
		/// 例えば、srcIndexBiasに10が指定されていると、srcIndeciesに記憶されている値に+10加算したものがコピーされていきます。
		/// </remark>
		void Copy(uint32_t* srcIndecies, int numCopy, uint32_t copyStartAddrOffset, uint32_t srcIndexBias);

		/// <summary>
		/// インデックスバッファビューを取得。
		/// </summary>
		/// <returns></returns>
		const D3D12_INDEX_BUFFER_VIEW& GetView() const
		{
			return m_indexBufferView;
		}
		/// <summary>
		/// インデックスの数を取得。
		/// </summary>
		/// <returns>インデックスの数。</returns>
		int GetCount() const
		{
			return m_count;
		}
		/// <summary>
		/// インデックス数を設定。
		/// </summary>
		/// <param name="count"></param>
		void SetCount(int count)
		{
			m_count = count;
		}
		/// <summary>
		/// インデックスバッファのストライドを取得。
		/// </summary>
		/// ストライドというのは、１要素のサイズのこと。
		/// ここでは、一つのインデックスのサイズを取得することになる。
		/// 2か4が返ってきます。
		/// <returns></returns>
		UINT GetStrideInBytes() const
		{
			return static_cast<UINT>(m_strideInBytes);
		}
		/// <summary>
		/// インデックスバッファのサイズ(単位：バイト)を取得。
		/// </summary>
		/// <returns></returns>
		UINT GetSizeInBytes() const
		{
			return static_cast<UINT>(m_sizeInBytes);
		}
		/// <summary>
		/// ID3D12Resourceのアドレスを取得します。
		/// </summary>
		/// <returns></returns>
		ID3D12Resource* GetID3DResourceAddress() const
		{
			return m_indexBuffer;
		}
	private:

		/// <summary>
		/// 解放
		/// </summary>
		void Release();
	private:
		ID3D12Resource* m_indexBuffer = nullptr;	//インデックスバッファ。
		D3D12_INDEX_BUFFER_VIEW m_indexBufferView;	//インデックスバッファビュー。
		int m_count = 0;							//インデックスの数。
		int m_strideInBytes = 0;					//ストライド(単位：バイト)。
		int m_sizeInBytes = 0;						//サイズ(単位：バイト)。
	};
}