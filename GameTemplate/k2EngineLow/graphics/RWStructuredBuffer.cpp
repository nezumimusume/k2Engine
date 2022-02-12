#include "k2EngineLowPreCompile.h"
#include "StructuredBuffer.h"
#include "VertexBuffer.h"

namespace nsK2EngineLow {
	RWStructuredBuffer::~RWStructuredBuffer()
	{
		Release();
	}
	void RWStructuredBuffer::Release()
	{
		//アンマップ
		CD3DX12_RANGE readRange(0, 0);
		for( int i = 0; i < 2; i++){
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
	void RWStructuredBuffer::Init(int sizeOfElement, int numElement, void* initData, bool isAccessCPU)
	{
		Release();
		m_isAccessCPU = isAccessCPU;
		m_sizeOfElement = sizeOfElement;
		m_numElement = numElement;
		auto device = g_graphicsEngine->GetD3DDevice();
		D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(m_sizeOfElement * m_numElement);
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		int bufferNo = 0;

		D3D12_HEAP_PROPERTIES prop{};
		if (m_isAccessCPU) {
			// CPUアクセスする？
			prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
			prop.CreationNodeMask = 1;
			prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
			prop.Type = D3D12_HEAP_TYPE_CUSTOM;
			prop.VisibleNodeMask = 1;
		}
		else {
			// CPUアクセスしない。
			prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			prop.CreationNodeMask = 1;
			prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			prop.Type = D3D12_HEAP_TYPE_DEFAULT;
			prop.VisibleNodeMask = 1;
		}
		// CPUからアクセスする場合はGPUがアクセスしているバッファを書き換える可能性があるので、
		// ダブルバッファにする。
		int numBuffer = m_isAccessCPU ? 2 : 1;
		for (int bufferNo = 0; bufferNo < numBuffer; bufferNo++ ) {
			auto& buffer = m_buffersOnGPU[bufferNo];
			device->CreateCommittedResource(
				&prop,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
				nullptr,
				IID_PPV_ARGS(&buffer)
			);
			//構造化バッファをCPUからアクセス可能な仮想アドレス空間にマッピングする。
			//マップ、アンマップのオーバーヘッドを軽減するためにはこのインスタンスが生きている間は行わない。
			if(m_isAccessCPU)
			{
				// 初期データのコピーができるのは
				CD3DX12_RANGE readRange(0, 0);        //     intend to read from this resource on the CPU.
				buffer->Map(0, &readRange, reinterpret_cast<void**>(&m_buffersOnCPU[bufferNo]));
				memcpy(m_buffersOnCPU[bufferNo], initData, m_sizeOfElement * m_numElement);
			}

			bufferNo++;
		}
		m_isInited = true;
	}
	void RWStructuredBuffer::Init(const VertexBuffer& vb, bool isUpdateByCPU)
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
	void RWStructuredBuffer::Init(const IndexBuffer& ib, bool isUpdateByCPU)
	{
		m_sizeOfElement = ib.GetStrideInBytes();
		m_numElement = ib.GetSizeInBytes() / m_sizeOfElement;
		if (isUpdateByCPU) {
			//未対応。
			std::abort();
		}
		else {

			for (auto& gpuBuffer : m_buffersOnGPU) {
				gpuBuffer = ib.GetID3DResourceAddress();
				gpuBuffer->AddRef();
			}
			//CPUからは変更できないのでマップしない。
			for (auto& cpuBuffer : m_buffersOnCPU) {
				cpuBuffer = nullptr;
			}
		}
		m_isInited = true;
	}
	ID3D12Resource* RWStructuredBuffer::GetD3DResoruce()
	{
		auto backBufferIndex = g_graphicsEngine->GetBackBufferIndex();
		// CPUからアクセスができない場合は0番目しか使わない。
		backBufferIndex = m_isAccessCPU ? backBufferIndex : 0;
		return m_buffersOnGPU[backBufferIndex];
	}
	void* RWStructuredBuffer::GetResourceOnCPU()
	{
		auto backBufferIndex = g_graphicsEngine->GetBackBufferIndex();
		// CPUからアクセスができない場合は0番目しか使わない。
		backBufferIndex = m_isAccessCPU ? backBufferIndex : 0;
		return m_buffersOnCPU[backBufferIndex];
	}
	void RWStructuredBuffer::RegistUnorderAccessView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo)
	{
		if (!m_isInited) {
			return;
		}
		// CPUからアクセスができない場合は0番目しか使わない。
		bufferNo = m_isAccessCPU ? bufferNo : 0;
		auto device = g_graphicsEngine->GetD3DDevice();
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
		desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Buffer.NumElements = static_cast<UINT>(m_numElement);
		desc.Buffer.StructureByteStride = m_sizeOfElement;
		device->CreateUnorderedAccessView(
			m_buffersOnGPU[bufferNo],
			nullptr,
			&desc,
			descriptorHandle
		);
	}
	void RWStructuredBuffer::RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo)
	{
		if (!m_isInited) {
			return;
		}
		// CPUからアクセスができない場合は0番目しか使わない。
		bufferNo = m_isAccessCPU ? bufferNo : 0;
		auto device = g_graphicsEngine->GetD3DDevice();
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
}
