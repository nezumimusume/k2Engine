#include "k2EngineLowPreCompile.h"
#include "StructuredBuffer.h"

namespace nsK2EngineLow {
	StructuredBuffer::~StructuredBuffer()
	{
		Release();
	}
	void StructuredBuffer::Release()
	{
		//アンマーップ
		CD3DX12_RANGE readRange(0, 0);
		for (int i = 0; i < 2; i++) {
			if (m_buffersOnGPU[i]) {
				if (m_buffersOnCPU[i]) {
					// メインメモリにマップしているのでアンマップを行う。
					m_buffersOnGPU[i]->Unmap(0, &readRange);
				}
				ReleaseD3D12Object(m_buffersOnGPU[i]);
				m_buffersOnGPU[i] = nullptr;
			}
		}
	}
	void StructuredBuffer::Init(int sizeOfElement, int numElement, void* initData, bool isDoubleBuffer)
	{
		Release();
		if (numElement == 0) {
			return;
		}
		m_isDoubleBuffer = isDoubleBuffer;
		m_sizeOfElement = sizeOfElement;
		m_numElement = numElement;
		auto device = g_graphicsEngine->GetD3DDevice();

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto rDesc = CD3DX12_RESOURCE_DESC::Buffer(m_sizeOfElement * m_numElement);
		int numBuffer = m_isDoubleBuffer ? 2 : 1;
		for( int bufferNo = 0; bufferNo < numBuffer; bufferNo++ ){		
			auto hr = device->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&rDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_buffersOnGPU[bufferNo])
			);


			//構造化バッファをCPUからアクセス可能な仮想アドレス空間にマッピングする。
			//マップ、アンマップのオーバーヘッドを軽減するためにはこのインスタンスが生きている間は行わない。
			{
				CD3DX12_RANGE readRange(0, 0);        //     intend to read from this resource on the CPU.
				m_buffersOnGPU[bufferNo]->Map(0, &readRange, reinterpret_cast<void**>(&m_buffersOnCPU[bufferNo]));
			}
			if (initData != nullptr) {
				memcpy(m_buffersOnCPU[bufferNo], initData, m_sizeOfElement * m_numElement);
			}
		}
		m_isInited = true;
	}
	void StructuredBuffer::Init(const VertexBuffer& vb, bool isUpdateByCPU)
	{
		Release();
		m_sizeOfElement = vb.GetStrideInBytes();
		m_numElement = vb.GetSizeInBytes() / m_sizeOfElement;
		if (isUpdateByCPU) {
			//未対応。
			std::abort();
		}
		else {
			for (auto& gpuBuffer : m_buffersOnGPU) {
				gpuBuffer = vb.GetID3DResourceAddress();
				gpuBuffer->AddRef();
			}
			//CPUからは変更できないのでマップしない。
			for (auto& cpuBuffer : m_buffersOnCPU) {
				cpuBuffer = nullptr;
			}
		}
		m_isInited = true;
	}
	void StructuredBuffer::Update(void* data)
	{
		auto backBufferIndex = GetBackBufferNo();
		memcpy(m_buffersOnCPU[backBufferIndex], data, m_numElement * m_sizeOfElement);
	}
	ID3D12Resource* StructuredBuffer::GetD3DResoruce()
	{
		auto backBufferIndex = GetBackBufferNo();
		return m_buffersOnGPU[backBufferIndex];
	}
	void StructuredBuffer::RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo)
	{
		if (!m_isInited) {
			return;
		}
		auto device = g_graphicsEngine->GetD3DDevice();
		
		bufferNo = m_isDoubleBuffer ? bufferNo : 0;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = static_cast<UINT>(m_numElement);
		srvDesc.Buffer.StructureByteStride = m_sizeOfElement;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		device->CreateShaderResourceView(
			m_buffersOnGPU[bufferNo],
			&srvDesc,
			descriptorHandle
		);
	}
	int StructuredBuffer::GetBackBufferNo() const
	{
		if (m_isDoubleBuffer) {
			// 内部でダブルバッファ化している場合はエンジンのバックバッファの番号と合わせる。
			return g_graphicsEngine->GetBackBufferIndex();
		}
		// ダブルバッファ化していない。
		return 0;
	}
}
