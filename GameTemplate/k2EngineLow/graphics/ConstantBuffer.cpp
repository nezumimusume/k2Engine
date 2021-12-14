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
			}
		}
	}
	void ConstantBuffer::Init(int size, void* srcData)
	{
		Release();
		m_size = size;

		//D3D�f�o�C�X���擾�B
		auto device = g_graphicsEngine->GetD3DDevice();


		//�萔�o�b�t�@��256�o�C�g�A���C�����g���v�������̂ŁA256�̔{���ɐ؂�グ��B
		m_allocSize = (size + 256) & 0xFFFFFF00;
		//�萔�o�b�t�@�̍쐬�B
		int bufferNo = 0;
		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto rDesc = CD3DX12_RESOURCE_DESC::Buffer(m_allocSize);
		for (auto& cb : m_constantBuffer) {
			device->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&rDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&cb)
			);
			//�萔�o�b�t�@��CPU����A�N�Z�X�\�ȉ��z�A�h���X��ԂɃ}�b�s���O����B
			//�}�b�v�A�A���}�b�v�̃I�[�o�[�w�b�h���y�����邽�߂ɂ͂��̃C���X�^���X�������Ă���Ԃ͍s��Ȃ��B
			{
				CD3DX12_RANGE readRange(0, 0);        //     intend to read from this resource on the CPU.
				cb->Map(0, &readRange, reinterpret_cast<void**>(&m_constBufferCPU[bufferNo]));
			}
			if (srcData != nullptr) {
				memcpy(m_constBufferCPU[bufferNo], srcData, m_size);
			}
			bufferNo++;
		}
		//���p�\�ɂ���B
		m_isValid = true;
	}
	void ConstantBuffer::RegistConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo)
	{
		//D3D�f�o�C�X���擾�B
		auto device = g_graphicsEngine->GetD3DDevice();
		D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
		desc.BufferLocation = m_constantBuffer[bufferNo]->GetGPUVirtualAddress();
		desc.SizeInBytes = m_allocSize;
		device->CreateConstantBufferView(&desc, descriptorHandle);
	}
	void ConstantBuffer::RegistConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle)
	{
		auto backBufferIndex = g_graphicsEngine->GetBackBufferIndex();
		RegistConstantBufferView(descriptorHandle, backBufferIndex);
	}
	void ConstantBuffer::CopyToVRAM(void* data)
	{
		auto backBufferIndex = g_graphicsEngine->GetBackBufferIndex();
		memcpy(m_constBufferCPU[backBufferIndex], data, m_size);
	}
	D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetGPUVirtualAddress()
	{
		auto backBufferIndex = g_graphicsEngine->GetBackBufferIndex();
		return m_constantBuffer[backBufferIndex]->GetGPUVirtualAddress();
	}
}