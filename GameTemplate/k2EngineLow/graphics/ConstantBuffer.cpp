#include "k2EngineLowPreCompile.h"
#include "ConstantBuffer.h"

namespace nsK2EngineLow {
	ConstantBuffer::~ConstantBuffer()
	{
		Release();
	}
	void ConstantBuffer::Release()
	{
		CD3DX12_RANGE readRange(0, 0);
		for (auto& cb : m_constantBuffer) {
			if (cb != nullptr) {
				cb->Unmap(0, &readRange);
				ReleaseD3D12Object(cb);
				cb = nullptr;
			}
		}
	}
	void ConstantBuffer::Init(int size, const void* srcData, bool isDoubleBuffer )
	{
		Release();
		m_isDoubleBuffer = isDoubleBuffer;
		m_size = size;
		//D3Dデバイスを取得。
		auto device = g_graphicsEngine->GetD3DDevice();

		//定数バッファは256バイトアライメントが要求されるので、256の倍数に切り上げる。
		m_allocSize = (size + 256) & 0xFFFFFF00;
		//定数バッファの作成。
		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto rDesc = CD3DX12_RESOURCE_DESC::Buffer(m_allocSize);
		int numBuffer = isDoubleBuffer ? 2 : 1;
		for (int i = 0; i < numBuffer; i++ ){
			auto& cb = m_constantBuffer[i];
			device->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&rDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&cb)
			);
			//定数バッファをCPUからアクセス可能な仮想アドレス空間にマッピングする。
			//マップ、アンマップのオーバーヘッドを軽減するためにはこのインスタンスが生きている間は行わない。
			{
				CD3DX12_RANGE readRange(0, 0);        //     intend to read from this resource on the CPU.
				cb->Map(0, &readRange, reinterpret_cast<void**>(&m_constBufferCPU[i]));
			}
			if (srcData != nullptr) {
				memcpy(m_constBufferCPU[i], srcData, m_size);
			}
		}
		//利用可能にする。
		m_isValid = true;
	}
	int ConstantBuffer::GetBackBufferNo() const
	{
		if (m_isDoubleBuffer) {
			// 内部でダブルバッファ化している場合はエンジンのバックバッファの番号と合わせる。
			return g_graphicsEngine->GetBackBufferIndex();
		}
		// ダブルバッファ化していない。
		return 0;
	}
	void ConstantBuffer::RegistConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo)
	{
		//D3Dデバイスを取得。
		auto device = g_graphicsEngine->GetD3DDevice();
		D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
		desc.BufferLocation = m_constantBuffer[bufferNo]->GetGPUVirtualAddress();
		desc.SizeInBytes = m_allocSize;
		device->CreateConstantBufferView(&desc, descriptorHandle);
	}
	void ConstantBuffer::RegistConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle)
	{
		auto backBufferIndex = GetBackBufferNo();
		RegistConstantBufferView(descriptorHandle, backBufferIndex);
	}
	void ConstantBuffer::CopyToVRAM(void* data)
	{
		auto backBufferIndex = GetBackBufferNo();
		memcpy(m_constBufferCPU[backBufferIndex], data, m_size);
	}
	D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetGPUVirtualAddress()
	{
		auto backBufferIndex = GetBackBufferNo();
		return m_constantBuffer[backBufferIndex]->GetGPUVirtualAddress();
	}
}