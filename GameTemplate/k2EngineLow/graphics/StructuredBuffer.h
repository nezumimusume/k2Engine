#pragma once

namespace nsK2EngineLow {

	/// <summary>
	/// 構造化バッファ
	/// </summary>
	class StructuredBuffer : public IShaderResource {
	public:
		/// <summary>
		/// デストラクタ。
		/// </summary>
		~StructuredBuffer();
		/// <summary>
		/// 構造化バッファを初期化。
		/// </summary>
		/// <param name="sizeOfElement">エレメントのサイズ。</param>
		/// <param name="numElement">エレメントの数。</param>
		/// <param name="initData">初期データ。</param>
		/// <param name="isDoubleBuffer">内部でダブルバッファ化する？。</param>
		void Init(int sizeOfElement, int numElement, void* initData, bool isDoubleBuffer = true);
		/// <summary>
		/// 頂点バッファを使って構造化バッファを初期化する。
		/// </summary>
		/// <param name="vb"></param>
		/// <param name="isUpdateByCPU"></param>
		void Init(const VertexBuffer& vb, bool isUpdateByCPU);
		
		/// <summary>
		/// SRVに登録。
		/// </summary>
		/// <param name="descriptorHandle"></param>
		void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo) override;
		/// <summary>
		/// 構造化バッファの内容を更新。
		/// </summary>
		/// <param name="data"></param>
		void Update(void* data);
		/// <summary>
		/// 初期化されているか判定。
		/// </summary>
		/// <returns></returns>
		bool IsInited() const
		{
			return m_isInited;
		}
		/// <summary>
		/// D3Dリソースを取得。
		/// </summary>
		/// <returns></returns>
		ID3D12Resource* GetD3DResoruce();
	private:
		/// <summary>
		/// 解放処理。
		/// </summary>
		void Release();
		/// <summary>
		/// バックバッファの番号を取得。
		/// </summary>
		/// <returns></returns>
		int GetBackBufferNo() const;
	private:
		ID3D12Resource* m_buffersOnGPU[2] = { nullptr };
		void* m_buffersOnCPU[2] = { nullptr };		//CPU側からアクセスできるするストラクチャバッファのアドレス。
		int m_numElement = 0;						//要素数。
		int m_sizeOfElement = 0;					//エレメントのサイズ。
		bool m_isInited = false;					//初期化済み？
		bool m_isDoubleBuffer = true;
	};
}