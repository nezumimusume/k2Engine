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
	void IndexBuffer::Copy(uint16_t* srcIndecies, int numCopy, uint32_t copyStartAddrOffset, uint32_t srcIndexBias)
	{
		uint8_t* pData;
		uint32_t* pData32;
		m_indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData));
		// コピー開始位置をオフセットする。
		pData += copyStartAddrOffset;
		pData32 = (uint32_t*)pData;
		numCopy = numCopy == 0 ? m_count : numCopy;
		for (int i = 0; i < numCopy; i++) {
			pData32[i] = srcIndecies[i] + srcIndexBias;
		}
		m_indexBuffer->Unmap(0, nullptr);
	}
	void IndexBuffer::Copy(uint32_t* srcIndecies, int numCopy, uint32_t copyStartAddrOffset, uint32_t srcIndexBias)
	{
		uint8_t* pData;
		uint32_t* pData32;
		m_indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData));
		// コピー開始位置をオフセットする。
		pData += copyStartAddrOffset;
		pData32 = (uint32_t*)pData;
		numCopy = numCopy == 0 ? m_count : numCopy;
		for (int i = 0; i < numCopy; i++) {
			pData32[i] = srcIndecies[i] + srcIndexBias;
		}
		m_indexBuffer->Unmap(0, nullptr);
	}
}