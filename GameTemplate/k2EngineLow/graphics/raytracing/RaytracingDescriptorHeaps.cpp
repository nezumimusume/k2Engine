#include "k2EngineLowPreCompile.h"
#include "RaytracingDescriptorHeaps.h"
#include "graphics/Material.h"

namespace nsK2EngineLow {
	namespace raytracing {
		void DescriptorHeaps::Init(
			int bufferNo,
			World& world,
			GPUBuffer& outputBuffer,
			ConstantBuffer& rayGeneCB,
			Texture& skycubeBox,
			StructuredBuffer& expandSRV
		)
		{

			//���C�g���̏o�͐���f�B�X�N���v�^�q�[�v�ɓo�^����B
			m_srvUavCbvHeap.Init(
				world.GetNumInstance() * (int)ESRV_OneEntry::eNum ,
				1,
				1,
				1
			);
			m_srvUavCbvHeap.RegistUnorderAccessResource(0, outputBuffer);
			m_srvUavCbvHeap.RegistConstantBuffer(0, rayGeneCB);
			int regNo = 0;
			world.QueryInstances(bufferNo, [&](Instance& instance)
			{
				m_srvUavCbvHeap.RegistShaderResource(
					regNo + (int)ESRV_OneEntry::eTLAS,
					world.GetTLASBuffer(bufferNo)
				);
				// �A���x�h�}�b�v���f�B�X�N���v�^�q�[�v�ɓo�^�B
				m_srvUavCbvHeap.RegistShaderResource(
					regNo + (int)ESRV_OneEntry::eAlbedoMap,
					instance.m_material->GetAlbedoMap()
				);
				// �@���}�b�v���f�B�X�N���v�^�q�[�v�ɓo�^�B
				m_srvUavCbvHeap.RegistShaderResource(
					regNo + (int)ESRV_OneEntry::eNormalMap,
					instance.m_material->GetNormalMap()
				);
				// �X�y�L�����}�b�v���f�B�X�N���v�^�q�[�v�ɓo�^�B
				m_srvUavCbvHeap.RegistShaderResource(
					regNo + (int)ESRV_OneEntry::eSpecularMap,
					instance.m_material->GetSpecularMap()
				);

				// ���t���N�V�����}�b�v���f�B�X�N���v�^�q�[�v�ɓo�^�B
				m_srvUavCbvHeap.RegistShaderResource(
					regNo + (int)ESRV_OneEntry::eReflectionMap,
					instance.m_material->GetReflectionMap()
				);

				// ���܃}�b�v���f�B�X�N���v�^�q�[�v�ɓo�^�B
				m_srvUavCbvHeap.RegistShaderResource(
					regNo + (int)ESRV_OneEntry::eRefractionMap,
					instance.m_material->GetRefractionMap()
				);
				// ���_�o�b�t�@���f�B�X�N���v�^�q�[�v�ɓo�^�B
				m_srvUavCbvHeap.RegistShaderResource(
					regNo + (int)ESRV_OneEntry::eVertexBuffer,
					instance.m_vertexBufferRWSB
				);
				// �C���f�b�N�X�o�b�t�@���f�B�X�N���v�^�q�[�v�ɓo�^�B
				m_srvUavCbvHeap.RegistShaderResource(
					regNo + (int)ESRV_OneEntry::eIndexBuffer,
					instance.m_indexBufferRWSB
				);
				// �X�J�C�L���[�u�{�b�N�X
				m_srvUavCbvHeap.RegistShaderResource(
					regNo + (int)ESRV_OneEntry::eSkyCubeBox,
					skycubeBox
				);
				// �g��SRV
				m_srvUavCbvHeap.RegistShaderResource(
					regNo + (int)ESRV_OneEntry::eExpandShaderResrouce,
					expandSRV
				);
				
				regNo += (int)ESRV_OneEntry::eNum;

			});

			//�T���v���X�e�[�g�̈����͉��B
			D3D12_SAMPLER_DESC samplerDesc = {};
			samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			samplerDesc.MipLODBias = 0;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
			samplerDesc.BorderColor[0] = 1.0f;
			samplerDesc.BorderColor[1] = 1.0f;
			samplerDesc.BorderColor[2] = 1.0f;
			samplerDesc.BorderColor[3] = 1.0f;
			samplerDesc.MinLOD = 0.0f;
			samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;

			//�T���v���X�e�[�g���f�B�X�N���v�^�q�[�v�ɓo�^����B
			m_samplerDescriptorHeap.RegistSamplerDesc(0, samplerDesc);
			// ���C�g���̃G���W�����Ń_�u���o�b�t�@�����Ă���̂ŁADescriptorHeap�̓����Ń_�u���o�b�t�@�ɂ͂��Ȃ��B
			m_samplerDescriptorHeap.CommitSamperHeap(false);
			m_srvUavCbvHeap.Commit(false);
		}
	}
}