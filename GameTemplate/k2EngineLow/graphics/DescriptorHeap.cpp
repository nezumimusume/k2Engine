#include "k2EngineLowPreCompile.h"
#include "DescriptorHeap.h"

namespace nsK2EngineLow {
	DescriptorHeap::DescriptorHeap()
	{
		m_shaderResources.resize(MAX_SHADER_RESOURCE);
		m_uavResoruces.resize(MAX_SHADER_RESOURCE);
		m_constantBuffers.resize(MAX_CONSTANT_BUFFER);
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
	DescriptorHeap::~DescriptorHeap()
	{
		Release();
	}
	void DescriptorHeap::Release()
	{
		ReleaseD3D12Object(m_descriptorHeap);
	}
	void DescriptorHeap::CommitSamperHeap()
	{
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


		for (int bufferNo = 0; bufferNo < 2; bufferNo++) {
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
	int g_numDescriptorHeap = 0;
	void DescriptorHeap::Commit()
	{
		Release();
		const auto& d3dDevice = g_graphicsEngine->GetD3DDevice();
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};

		srvHeapDesc.NumDescriptors = (m_numShaderResource + m_numConstantBuffer + m_numUavResource) * 2;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;


		auto hr = d3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_descriptorHeap));
		g_numDescriptorHeap++;
		if (FAILED(hr)) {
			MessageBox(nullptr, L"DescriptorHeap::Commit ディスクリプタヒープの作成に失敗しました。", L"エラー", MB_OK);
			std::abort();
		}

		//定数バッファやシェーダーリソースのディスクリプタをヒープに書き込んでいく。
		auto cpuHandle = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		auto gpuHandle = m_descriptorHeap->GetGPUDescriptorHandleForHeapStart();
		for (int bufferNo = 0; bufferNo < 2; bufferNo++) {
			//定数バッファを登録していく。
			for (int i = 0; i < m_numConstantBuffer; i++) {
				//@todo bug
				if (m_constantBuffers[i] != nullptr) {
					m_constantBuffers[i]->RegistConstantBufferView(cpuHandle, bufferNo);
				}
				//次に進める。
				cpuHandle.ptr += g_graphicsEngine->GetCbrSrvDescriptorSize();
			}

			//続いてシェーダーリソース。
			for (int i = 0; i < m_numShaderResource; i++) {
				if (m_shaderResources[i] != nullptr) {
					m_shaderResources[i]->RegistShaderResourceView(cpuHandle, bufferNo);
				}
				//次に進める。
				cpuHandle.ptr += g_graphicsEngine->GetCbrSrvDescriptorSize();
			}

			//続いてUAV。
			for (int i = 0; i < m_numUavResource; i++) {
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
			m_uavGpuDescriptorStart[bufferNo].ptr += (UINT64)g_graphicsEngine->GetCbrSrvDescriptorSize() * (m_numShaderResource + m_numConstantBuffer);

			gpuHandle.ptr += (UINT64)g_graphicsEngine->GetCbrSrvDescriptorSize() * (m_numShaderResource + m_numConstantBuffer + m_numUavResource);
		}
	}
}