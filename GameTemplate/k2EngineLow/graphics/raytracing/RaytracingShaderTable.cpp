#include "k2EngineLowPreCompile.h"
#include "RaytracingShaderTable.h"
#include "RaytracingPSO.h"

#define align_to(_alignment, _val) (((_val + _alignment - 1) / _alignment) * _alignment)

namespace nsK2EngineLow {
	namespace raytracing {

		void ShaderTable::CountupNumGeyGenAndMissAndHitShader()
		{
			//�e�V�F�[�_�[�̐��𒲍��B
			m_numRayGenShader = 0;
			m_numMissShader = 0;
			m_numHitShader = 0;
			for (auto& shaderData : shaderDatas) {
				if (shaderData.category == eShaderCategory_RayGenerator) m_numRayGenShader++;
				if (shaderData.category == eShaderCategory_Miss) m_numMissShader++;
				if (shaderData.category == eShaderCategory_ClosestHit) m_numHitShader++;
			}
		}

		void ShaderTable::CalcShaderTableEntrySize()
		{
			//�V�F�[�_�[�e�[�u���ɓo�^�����f�[�^�̂P�v�f���̃T�C�Y���v�Z�B
			//�V�F�[�_�[���ʎq�B
			m_shaderTableEntrySize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
			//�f�B�X�N���v�^�e�[�u���̃A�h���X�B
			m_shaderTableEntrySize += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) * eHitShaderDescriptorTable_Num; // The hit shader
			//�A���C�����g�����낦��B
			m_shaderTableEntrySize = align_to(D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT, m_shaderTableEntrySize);
		}
		void ShaderTable::Init(
			const World& world,
			const PSO& pso,
			const DescriptorHeaps& descriptorHeaps
		)
		{

			//�e�V�F�[�_�[�̐����J�E���g����B
			CountupNumGeyGenAndMissAndHitShader();

			//�V�F�[�_�[�e�[�u���ɓo�^�����f�[�^�̂P�v�f���̃T�C�Y���v�Z�B
			CalcShaderTableEntrySize();

			//�V�F�[�_�[�e�[�u���̃T�C�Y���v�Z�B
			int shaderTableSize = m_shaderTableEntrySize * (m_numRayGenShader + m_numMissShader + (m_numHitShader * world.GetNumInstance()));

			auto d3dDevice = g_graphicsEngine->GetD3DDevice();
			//�V�F�[�_�[�e�[�u���p�̃o�b�t�@���쐬�B
			m_shaderTable = CreateBuffer(d3dDevice, shaderTableSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, kUploadHeapProps);

			//�o�b�t�@���V�X�e���������Ƀ}�b�v����B
			uint8_t* pData;
			m_shaderTable->Map(0, nullptr, (void**)&pData);

			CComPtr< ID3D12StateObjectProperties > pRtsoProps;
			pso.QueryInterface(pRtsoProps);

			uint8_t* pCurret = pData;

			auto ds_size_cbv_srv_uav = g_graphicsEngine->GetCbrSrvDescriptorSize();
			auto hitGroup_pbrCameraRaySrvHeapStride =
				ds_size_cbv_srv_uav * (int)ESRV_OneEntry::eNum;

			const auto& srvUavCbvDescriptorHeap = descriptorHeaps.GetSrvUavCbvDescriptorHeap();
			const auto& samplerDescriptorHeap = descriptorHeaps.GetSamplerDescriptorHeap();

			uint64_t hitGroup_pbrCameraRaySrvHeapStart = srvUavCbvDescriptorHeap.Get()->GetGPUDescriptorHandleForHeapStart().ptr + ds_size_cbv_srv_uav;
			//�V�F�[�_�[�e�[�u���ɃV�F�[�_�[��o�^����֐��B
			auto RegistShaderTblFunc = [&](const ShaderData& shaderData, EShaderCategory registCategory, Instance* instance) {
				if (shaderData.category == registCategory) {
					//�܂��V�F�[�_�[ID��ݒ肷��B
					void* pShaderId = nullptr;
					if (registCategory == eShaderCategory_ClosestHit) {
						pShaderId = pRtsoProps->GetShaderIdentifier(hitGroups[shaderData.hitgroup].name);
					}
					else {
						pShaderId = pRtsoProps->GetShaderIdentifier(shaderData.entryPointName);
					}
					memcpy(pCurret, pShaderId, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
					uint8_t* pDst = pCurret + D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
					if (shaderData.useLocalRootSignature == eLocalRootSignature_Raygen) {
						//�f�t�H���g�̃��[�g�V�O�l�`�����g�p����ꍇ�A�V�F�[�_�[ID�̌��Ƀf�B�X�N���v�^�q�[�v�ւ̃A�h���X��ݒ肷��B
						*(uint64_t*)(pDst) = srvUavCbvDescriptorHeap.Get()->GetGPUDescriptorHandleForHeapStart().ptr;
					}
					if (shaderData.useLocalRootSignature == eLocalRootSignature_PBRMaterialHit) {
						//SRV_CBV�̃f�B�X�N���v�^�q�[�v				
						*(uint64_t*)(pDst) = hitGroup_pbrCameraRaySrvHeapStart;
						pDst += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE);
						*(uint64_t*)(pDst) = samplerDescriptorHeap.GetSamplerResourceGpuDescritorStartHandle().ptr;

					}
					//���B
					pCurret += m_shaderTableEntrySize;
				}
			};
			// ���C�W�F�l���[�V�����V�F�[�_�[���e�[�u���ɓo�^���Ă����B
			for (auto& shader : shaderDatas) {
				RegistShaderTblFunc(shader, eShaderCategory_RayGenerator, nullptr);
			}
			// �����ă~�X�V�F�[�_�[�B
			for (auto& shader : shaderDatas) {
				RegistShaderTblFunc(shader, eShaderCategory_Miss, nullptr);
			}
			//�Ō�Ƀq�b�g�V�F�[�_�[�B�q�b�g�V�F�[�_�[�̓q�b�g�V�F�[�_�[�̐��@�~�@�C���X�^���X�̐������o�^����B
			world.QueryInstances([&](Instance& instance) {
				for (auto& shader : shaderDatas) {
					RegistShaderTblFunc(shader, eShaderCategory_ClosestHit, &instance);
				};
				hitGroup_pbrCameraRaySrvHeapStart += hitGroup_pbrCameraRaySrvHeapStride;	//��
			});

			//Unmap
			m_shaderTable->Unmap(0, nullptr);
		}
	}
}