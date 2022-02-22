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

		// �e�탁���o�ϐ�������������B
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
			MessageBox(nullptr, L"DescriptorHeap::Commit �f�B�X�N���v�^�q�[�v�̍쐬�Ɏ��s���܂����B", L"�G���[", MB_OK);
			std::abort();
		}

		int numBuffer = isDoubleBuffer ? 2 : 1;
		for (int bufferNo = 0; bufferNo < numBuffer; bufferNo++) {
			auto cpuHandle = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
			auto gpuHandle = m_descriptorHeap->GetGPUDescriptorHandleForHeapStart();
			for (int i = 0; i < m_numSamplerDesc; i++) {
				//�T���v���X�e�[�g���f�B�X�N���v�^�q�[�v�ɓo�^���Ă����B
				d3dDevice->CreateSampler(&m_samplerDescs[i], cpuHandle);
				cpuHandle.ptr += g_graphicsEngine->GetSapmerDescriptorSize();
			}
			m_samplerGpuDescriptorStart[bufferNo] = gpuHandle;
		}

	}
	int DescriptorHeap::GetBackBufferNo() const
	{
		if (m_isDoubleBuffer) {
			// �����Ń_�u���o�b�t�@�����Ă���ꍇ�̓G���W���̃o�b�N�o�b�t�@�̔ԍ��ƍ��킹��B
			return g_graphicsEngine->GetBackBufferIndex();
		}
		// �_�u���o�b�t�@�����Ă��Ȃ��B
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
			MessageBox(nullptr, L"DescriptorHeap::Commit �f�B�X�N���v�^�q�[�v�̍쐬�Ɏ��s���܂����B", L"�G���[", MB_OK);
			std::abort();
		}

		//�萔�o�b�t�@��V�F�[�_�[���\�[�X�̃f�B�X�N���v�^���q�[�v�ɏ�������ł����B
		auto cpuHandle = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		auto gpuHandle = m_descriptorHeap->GetGPUDescriptorHandleForHeapStart();

		int numBuffer = isDoubleBuffer ? 2 : 1;
		for (int bufferNo = 0; bufferNo < numBuffer; bufferNo++) {
			//�萔�o�b�t�@��o�^���Ă����B
			for (int i = 0; i < m_numConstantBuffer; i++) {
				if (m_constantBuffers[i] != nullptr) {
					m_constantBuffers[i]->RegistConstantBufferView(cpuHandle, bufferNo);
				}
				//���ɐi�߂�B
				cpuHandle.ptr += g_graphicsEngine->GetCbrSrvDescriptorSize();
			}

			//�����ăV�F�[�_�[���\�[�X�B
			for (int i = 0; i < m_numSRV; i++) {
				if (m_shaderResources[i] != nullptr) {
					m_shaderResources[i]->RegistShaderResourceView(cpuHandle, bufferNo);
				}
				//���ɐi�߂�B
				cpuHandle.ptr += g_graphicsEngine->GetCbrSrvDescriptorSize();
			}

			//������UAV�B
			for (int i = 0; i < m_numUAV; i++) {
				if (m_uavResoruces[i] != nullptr) {
					m_uavResoruces[i]->RegistUnorderAccessView(cpuHandle, bufferNo);
				}
				//���ɐi�߂�B
				cpuHandle.ptr += g_graphicsEngine->GetCbrSrvDescriptorSize();
			}

			//�萔�o�b�t�@�̃f�B�X�N���v�^�q�[�v�̊J�n�n���h�����v�Z�B
			m_cbGpuDescriptorStart[bufferNo] = gpuHandle;
			//�V�F�[�_�[���\�[�X�̃f�B�X�N���v�^�q�[�v�̊J�n�n���h�����v�Z�B
			m_srGpuDescriptorStart[bufferNo] = gpuHandle;
			m_srGpuDescriptorStart[bufferNo].ptr += (UINT64)g_graphicsEngine->GetCbrSrvDescriptorSize() * m_numConstantBuffer;
			//UAV���\�[�X�̃f�B�X�N���v�^�q�[�v�̊J�n�n���h�����v�Z�B
			m_uavGpuDescriptorStart[bufferNo] = gpuHandle;
			m_uavGpuDescriptorStart[bufferNo].ptr += (UINT64)g_graphicsEngine->GetCbrSrvDescriptorSize() * (m_numSRV + m_numConstantBuffer);

			gpuHandle.ptr += (UINT64)g_graphicsEngine->GetCbrSrvDescriptorSize() * (m_numSRV + m_numConstantBuffer + m_numUAV);
		}
	}
}