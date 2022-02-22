#include "k2EngineLowPreCompile.h"
#include "DescriptorHeap.h"

namespace nsK2EngineLow {
	DescriptorHeap::DescriptorHeap()
	{
		Init(
			DEFAULT_MAX_SRV,
			DEFAULT_MAX_UAV,
			DEFAULT_MAX_CONSTANT_BUFFER,
			DEFAULT_MAX_SAMPLER_STATE
		);
		
	}
	DescriptorHeap::~DescriptorHeap()
	{
		Release();
	}
	void DescriptorHeap::Release()
	{
		ReleaseD3D12Object(m_descriptorHeap);
		m_descriptorHeap = nullptr;
	}
	void DescriptorHeap::Init(
		int maxSRV,
		int maxUAV,
		int maxConstantBuffer,
		int maxSamplerState
	)
	{
		Release();

		// 各種メンバ変数を初期化する。
		m_descriptorHeap = nullptr;
		m_numSRV = 0;
		m_numConstantBuffer = 0;
		m_numUAV = 0;
		m_numSamplerDesc = 0;

		m_maxSRV = maxSRV;
		m_maxUAV = maxUAV;
		m_maxConstantBuffer = maxConstantBuffer;
		m_maxSamplerState = maxSamplerState;

		m_shaderResources.resize(m_maxSRV);
		m_uavResoruces.resize(m_maxUAV);
		m_constantBuffers.resize(m_maxConstantBuffer);
		m_samplerDescs.resize(m_maxSamplerState);
		for (auto& srv : m_shaderResources) {
			srv = nullptr;
		}
		for (auto& uav : m_uavResoruces) {
			uav = nullptr;
		}
		for (auto& cbv : m_constantBuffers) {
			cbv = nullptr;
		}
	}
	void DescriptorHeap::CommitSamperHeap(bool isDoubleBuffer)
	{
		Release();

		m_isDoubleBuffer = isDoubleBuffer;
		const auto& d3dDevice = g_graphicsEngine->GetD3DDevice();
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};

		srvHeapDesc.NumDescriptors = m_numSamplerDesc * 2;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		auto hr = d3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_descriptorHeap));
		if (FAILED(hr)) {
			MessageBox(nullptr, L"DescriptorHeap::Commit ディスクリプタヒープの作成に失敗しました。", L"エラー", MB_OK);
			std::abort();
		}

		int numBuffer = isDoubleBuffer ? 2 : 1;
		for (int bufferNo = 0; bufferNo < numBuffer; bufferNo++) {
			auto cpuHandle = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
			auto gpuHandle = m_descriptorHeap->GetGPUDescriptorHandleForHeapStart();
			for (int i = 0; i < m_numSamplerDesc; i++) {
				//サンプラステートをディスクリプタヒープに登録していく。
				d3dDevice->CreateSampler(&m_samplerDescs[i], cpuHandle);
				cpuHandle.ptr += g_graphicsEngine->GetSapmerDescriptorSize();
			}
			m_samplerGpuDescriptorStart[bufferNo] = gpuHandle;
		}

	}
	int DescriptorHeap::GetBackBufferNo() const
	{
		if (m_isDoubleBuffer) {
			// 内部でダブルバッファ化している場合はエンジンのバックバッファの番号と合わせる。
			return g_graphicsEngine->GetBackBufferIndex();
		}
		// ダブルバッファ化していない。
		return 0;
	}
	void DescriptorHeap::Commit(bool isDoubleBuffer)
	{
		Release();
		m_isDoubleBuffer = isDoubleBuffer;
		const auto& d3dDevice = g_graphicsEngine->GetD3DDevice();
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};

		srvHeapDesc.NumDescriptors = (m_numSRV + m_numConstantBuffer + m_numUAV) * 2;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;


		auto hr = d3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_descriptorHeap));
		
		if (FAILED(hr)) {
			MessageBox(nullptr, L"DescriptorHeap::Commit ディスクリプタヒープの作成に失敗しました。", L"エラー", MB_OK);
			std::abort();
		}

		//定数バッファやシェーダーリソースのディスクリプタをヒープに書き込んでいく。
		auto cpuHandle = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		auto gpuHandle = m_descriptorHeap->GetGPUDescriptorHandleForHeapStart();

		int numBuffer = isDoubleBuffer ? 2 : 1;
		for (int bufferNo = 0; bufferNo < numBuffer; bufferNo++) {
			//定数バッファを登録していく。
			for (int i = 0; i < m_numConstantBuffer; i++) {
				if (m_constantBuffers[i] != nullptr) {
					m_constantBuffers[i]->RegistConstantBufferView(cpuHandle, bufferNo);
				}
				//次に進める。
				cpuHandle.ptr += g_graphicsEngine->GetCbrSrvDescriptorSize();
			}

			//続いてシェーダーリソース。
			for (int i = 0; i < m_numSRV; i++) {
				if (m_shaderResources[i] != nullptr) {
					m_shaderResources[i]->RegistShaderResourceView(cpuHandle, bufferNo);
				}
				//次に進める。
				cpuHandle.ptr += g_graphicsEngine->GetCbrSrvDescriptorSize();
			}

			//続いてUAV。
			for (int i = 0; i < m_numUAV; i++) {
				if (m_uavResoruces[i] != nullptr) {
					m_uavResoruces[i]->RegistUnorderAccessView(cpuHandle, bufferNo);
				}
				//次に進める。
				cpuHandle.ptr += g_graphicsEngine->GetCbrSrvDescriptorSize();
			}

			//定数バッファのディスクリプタヒープの開始ハンドルを計算。
			m_cbGpuDescriptorStart[bufferNo] = gpuHandle;
			//シェーダーリソースのディスクリプタヒープの開始ハンドルを計算。
			m_srGpuDescriptorStart[bufferNo] = gpuHandle;
			m_srGpuDescriptorStart[bufferNo].ptr += (UINT64)g_graphicsEngine->GetCbrSrvDescriptorSize() * m_numConstantBuffer;
			//UAVリソースのディスクリプタヒープの開始ハンドルを計算。
			m_uavGpuDescriptorStart[bufferNo] = gpuHandle;
			m_uavGpuDescriptorStart[bufferNo].ptr += (UINT64)g_graphicsEngine->GetCbrSrvDescriptorSize() * (m_numSRV + m_numConstantBuffer);

			gpuHandle.ptr += (UINT64)g_graphicsEngine->GetCbrSrvDescriptorSize() * (m_numSRV + m_numConstantBuffer + m_numUAV);
		}
	}
}