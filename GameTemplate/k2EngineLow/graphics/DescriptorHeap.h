#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// ディスクリプタヒープ。
	/// </summary>
	class DescriptorHeap : public Noncopyable {
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		DescriptorHeap();
		/// <summary>
		/// デストラクタ。
		/// </summary>
		~DescriptorHeap();
		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="maxSRV">ディスクリプタヒープに登録できるSRVの最大数</param>
		/// <param name="maxUAV">ディスクリプタヒープに登録できるUAVの最大数</param>
		/// <param name="maxConstantBuffer">ディスクリプタヒープに登録できる定数バッファの最大数</param>
		/// <param name="maxSamplerState">ディスクリプタヒープに登録できるサンプラステートの最大数</param>
		/// <param name=""></param>
		void Init(
			int maxSRV, 
			int maxUAV, 
			int maxConstantBuffer, 
			int maxSamplerState
		);
		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		ID3D12DescriptorHeap* Get() const;
		/// <summary>
		/// シェーダーリソースを記憶するための領域をリサイズ
		/// </summary>
		/// <remark>
		/// デフォルトで足りなくなった or 最適なサイズにしたい場合に利用してください。
		/// </remark>
		void ResizeShaderResource(int numSRV)
		{
			m_shaderResources.resize(numSRV);
		}
		/// <summary>
		/// 定数バッファを記憶するための領域を確保
		/// </summary>
		/// <remark>
		/// デフォルトで足りなくなった or 最適なサイズにしたい場合に利用してください。
		/// </remark>
		/// <param name="numCB"></param>
		void ResizeConstantBuffer(int numCB)
		{
			m_constantBuffers.resize(numCB);
		}
		/// <summary>
		/// UAVを記憶するための領域をリサイズ
		/// </summary>
		/// <remark>
		/// デフォルトで足りなくなった or 最適なサイズにしたい場合に利用してください。
		/// </remark>
		void ResizeUnorderAccessResource(int numUAV)
		{
			m_uavResoruces.resize(numUAV);
		}
		/// <summary>
		/// シェーダーリソースをディスクリプタヒープに登録。
		/// </summary>
		/// <param name="registerNo">
		/// レジスタ番号。-1が指定されたら、現在登録されているリソース数の次のレジスタが使用される。
		/// </param>
		/// <param name="sr">シェーダーリソース</param>
		void RegistShaderResource(int registerNo, IShaderResource& sr)
		{
			RegistResource(
				registerNo,
				&sr,
				&m_shaderResources.front(),
				m_numSRV,
				static_cast<int>(m_shaderResources.size()),
				L"DescriptorHeap::RegistShaderResource() レジスタ番号が範囲外です。"
			);
		}

		/// <summary>
		/// アンオーダーアクセスリソースを登録。
		/// </summary>
		/// <param name="registerNo">
		/// レジスタ番号。-1が指定されたら、現在登録されているリソース数の次のレジスタが使用される。
		/// </param>
		/// <param name="sr">アンオーダーリソース</param>
		void RegistUnorderAccessResource(int registerNo, IUnorderAccessResrouce& sr)
		{
			RegistResource(
				registerNo,
				&sr,
				&m_uavResoruces.front(),
				m_numUAV,
				static_cast<int>(m_uavResoruces.size()),
				L"DescriptorHeap::RegistUnorderAccessResource() レジスタ番号が範囲外です。"
			);
		}
		/// <summary>
		/// 定数バッファをディスクリプタヒープに登録。
		/// </summary>
		/// <param name="registerNo">
		/// レジスタ番号。-1が指定されたら、現在登録されているリソース数の次のレジスタが使用される。
		/// </param>
		/// <param name="cb">定数バッファ</param>
		void RegistConstantBuffer(int registerNo, ConstantBuffer& cb)
		{
			RegistResource(
				registerNo,
				&cb,
				&m_constantBuffers.front(),
				m_numConstantBuffer,
				static_cast<int>(m_constantBuffers.size()),
				L"DescriptorHeap::RegistConstantBuffer() レジスタ番号が範囲外です。"
			);
		}
		/// <summary>
		/// サンプラ定義をディスクリプタヒープに追加。
		/// </summary>
		/// <param name="registerNo">
		/// レジスタ番号。-1が指定されたら、現在登録されているリソース数の次のレジスタが使用される。
		/// </param>
		/// <param name="desc">
		/// サンプラ定義
		/// </param>
		void RegistSamplerDesc(int registerNo, const D3D12_SAMPLER_DESC& desc)
		{
			RegistResource(
				registerNo,
				desc,
				&m_samplerDescs.front(),
				m_numSamplerDesc,
				static_cast<int>(m_samplerDescs.size()),
				L"DescriptorHeap::RegistSamplerDesc() レジスタ番号が範囲外です。"
			);
		}
		/// <summary>
		/// ディスクリプタヒープへの登録を確定。
		/// </summary>
		void Commit(bool isDoubleBuffer = true);
		/// <summary>
		/// サンプラステート用のディスクリプタヒープへの登録。
		/// </summary>
		void CommitSamperHeap(bool isDoubleBuffer = true);
		/// <summary>
		/// バックバッファの番号を取得。
		/// </summary>
		/// <returns></returns>
		int GetBackBufferNo() const;
		/// <summary>
		/// 定数バッファのディスクリプタの開始ハンドルを取得。
		/// </summary>
		/// <returns></returns>
		D3D12_GPU_DESCRIPTOR_HANDLE GetConstantBufferGpuDescritorStartHandle() const;
		/// <summary>
		/// シェーダーリソースのディスクリプタの開始ハンドルを取得。
		/// </summary>
		/// <returns></returns>
		D3D12_GPU_DESCRIPTOR_HANDLE GetShaderResourceGpuDescritorStartHandle() const;
		/// <summary>
		/// Unorder Access リソースのディスクリプタの開始ハンドルを取得。
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE GetUavResourceGpuDescritorStartHandle() const;
		/// <summary>
		/// Samplerのディスクリプタの開始ハンドルを取得。
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE GetSamplerResourceGpuDescritorStartHandle() const;
		/// <summary>
		/// シェーダーリソースが一つでも登録されているか判定。
		/// </summary>
		/// <returns></returns>
		bool IsRegistShaderResource() const
		{
			return m_numSRV != 0;
		}
		/// <summary>
		/// 定数バッファが一つでも登録されているか判定。
		/// </summary>
		/// <returns></returns>
		bool IsRegistConstantBuffer() const
		{
			return m_numConstantBuffer != 0;
		}
		/// <summary>
		/// UAVリソースが一つでも登録されているか判定。
		/// </summary>
		/// <returns></returns>
		bool IsRegistUavResource() const
		{
			return m_numUAV != 0;
		}
		/// <summary>
		/// UAVディスクリプタが始まる配列番号を取得する。
		/// </summary>
		/// <remarks>
		/// UAVディスクリプタはSRVディスクリプタの次から登録されている。
		/// なので、SRVディスクリプタが10個登録されていれば、
		/// UAVディスクリプタは配列の10番目から登録されていることになる。
		/// この関数は現在レイトレエンジンで使用されている。
		/// </remarks>
		/// <returns></returns>
		int GetOffsetUAVDescriptorFromTableStart() const
		{
			return m_numSRV + m_numConstantBuffer;
		}
		/// <summary>
		/// SRVディスクリプタが始まる配列番号を取得する。
		/// </summary>
		/// <returns></returns>
		int GetOffsetSRVDescriptorFromTableStart() const
		{
			return m_numConstantBuffer;
		}
		/// <summary>
		/// 定数バッファディスクリプタが始まる配列番号を取得する。
		/// </summary>
		/// /// <remarks>
		/// 定数バッファディスクリプタはSRVディスクリプタとUAVディスクリプタの次から登録されている。
		/// なので、SRVディスクリプタが10個、UVAディスクリプタが5個登録されていれば、
		/// 定数バッファディスクリプタは配列の15番目から登録されていることになる。
		/// この関数は現在レイトレエンジンで使用されている。
		/// <returns></returns>
		int GetOffsetConstantBufferDescriptorFromTableStart() const
		{
			return m_numSRV + m_numUAV;
		}
	private:
		/// <summary>
		/// 解放
		/// </summary>
		void Release();
		/// <summary>
		/// リソースをディスクリプタヒープに登録。
		/// </summary>
		/// <param name="registerNo">登録番号</param>
		/// <param name="res">登録するリソース</param>
		/// <param name="resTbl">リソーステーブル。このテーブルにリソースが追加されます。</param>
		/// <param name="numRes">登録されているリソースの数。本関数を呼び出すと、この数が１インクリメントされます。</param>
		template<class T>
		void RegistResource(
			int registerNo,
			T res,
			T resTbl[],
			int& numRes,
			const int MAX_RESOURCE,
			const wchar_t* errorMessage
		)
		{
			if (registerNo == -1) {
				//-1が指定されていたら、現在登録されている末尾のリソースの次に登録される。
				registerNo = numRes;
			}
			if (registerNo < MAX_RESOURCE) {
				resTbl[registerNo] = res;
				if (numRes < registerNo + 1) {
					numRes = registerNo + 1;
				}
			}
			else {
				MessageBox(nullptr, errorMessage, L"エラー", MB_OK);
				std::abort();
			}
		}
	private:
		enum {
			DEFAULT_MAX_SRV = 128,				// シェーダーリソースの最大数のデフォルト値。
			DEFAULT_MAX_UAV = 128,				// UAVの最大数のデフォルト値
			DEFAULT_MAX_CONSTANT_BUFFER = 32,	// 定数バッファの最大数のデフォルト値。
			DEFAULT_MAX_SAMPLER_STATE = 16,		// サンプラステートの最大数のデフォルト値。
		};
		int m_maxSRV = DEFAULT_MAX_SRV;							// SRVの最大数。
		int m_maxUAV = DEFAULT_MAX_UAV;							// UAVの最大数。
		int m_maxConstantBuffer = DEFAULT_MAX_CONSTANT_BUFFER;	// 定数バッファの最大数
		int m_maxSamplerState = DEFAULT_MAX_SAMPLER_STATE;		// サンプラステートの最大数。
		int m_numSRV = 0;										// シェーダーリソースの数。
		int m_numConstantBuffer = 0;							// 定数バッファの数。
		int m_numUAV = 0;										// アンオーダーアクセスリソースの数。
		int m_numSamplerDesc = 0;								// サンプラの数。
		bool m_isDoubleBuffer = true;							// ダブルバッファ？
		ID3D12DescriptorHeap* m_descriptorHeap = { nullptr };	//ディスクリプタヒープ。
		std::vector<IShaderResource*> m_shaderResources;		//シェーダーリソース。
		std::vector < IUnorderAccessResrouce*> m_uavResoruces;	//UAVリソース。
		std::vector < ConstantBuffer*> m_constantBuffers;		//定数バッファ。
		std::vector< D3D12_SAMPLER_DESC > m_samplerDescs;		//サンプラステート。
		D3D12_GPU_DESCRIPTOR_HANDLE m_cbGpuDescriptorStart[2];						//定数バッファのディスクリプタヒープの開始ハンドル。
		D3D12_GPU_DESCRIPTOR_HANDLE m_srGpuDescriptorStart[2];						//シェーダーリソースのディスクリプタヒープの開始ハンドル。
		D3D12_GPU_DESCRIPTOR_HANDLE m_uavGpuDescriptorStart[2];						//UAVリソースのディスクリプタヒープの開始ハンドル。
		D3D12_GPU_DESCRIPTOR_HANDLE m_samplerGpuDescriptorStart[2];					//Samplerのでスクリプタヒープの開始ハンドル。
	};
}