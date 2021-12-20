#include "k2EngineLowPreCompile.h"
#include "IndexBuffer.h"


namespace nsK2EngineLow {

	IndexBuffer::~IndexBuffer()
	{
		Release();
	}
	void IndexBuffer::Release()
	{
		ReleaseD3D12Object(m_indexBuffer);
	}
	void IndexBuffer::Init(int size, int stride)
	{
		Release();
		if (stride == 2) {
			m_sizeInBytes = size * 2;
		}
		else {
			m_sizeInBytes = size;
		}
		auto d3dDevice = g_graphicsEngine->GetD3DDevice();
		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto rDesc = CD3DX12_RESOURCE_DESC::Buffer(m_sizeInBytes);
		auto hr = d3dDevice->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&rDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_indexBuffer));

		
		//インデックスバッファのビューを作成。
		m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();


		//ストライドは４バイト固定。
		m_strideInBytes = 4;
		m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_indexBufferView.SizeInBytes = m_sizeInBytes;

		m_count = m_sizeInBytes / m_strideInBytes;
	}
	void IndexBuffer::Copy(uint16_t* srcIndecies)
	{
		uint32_t* pData;
		m_indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData));
		for (int i = 0; i < m_count; i++) {
			pData[i] = srcIndecies[i];
		}
		m_indexBuffer->Unmap(0, nullptr);
	}
	void IndexBuffer::Copy(uint32_t* srcIndecies)
	{
		uint32_t* pData;
		m_indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData));
		for (int i = 0; i < m_count; i++) {
			pData[i] = srcIndecies[i];
		}
		m_indexBuffer->Unmap(0, nullptr);
	}
}