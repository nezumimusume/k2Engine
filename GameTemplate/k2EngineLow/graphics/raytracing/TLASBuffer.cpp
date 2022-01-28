#include "k2EngineLowPreCompile.h"
#include "TLASBuffer.h"

namespace nsK2EngineLow {
	namespace raytracing {
		extern 	ID3D12Resource* CreateBuffer(
			ID3D12Device5* pDevice,
			uint64_t size,
			D3D12_RESOURCE_FLAGS flags,
			D3D12_RESOURCE_STATES initState,
			const D3D12_HEAP_PROPERTIES& heapProps
		);

		extern const D3D12_HEAP_PROPERTIES kUploadHeapProps;

		extern const D3D12_HEAP_PROPERTIES kDefaultHeapProps;

		
		void TLASBuffer::Build(
			RenderContext& rc,
			const std::vector<InstancePtr>& instances,
			bool isUpdate
		){
			
			int numInstance = static_cast<int>(instances.size());
			auto d3dDevice = g_graphicsEngine->GetD3DDevice();
			m_inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
			m_inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
			m_inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
			m_inputs.NumDescs = numInstance;

			if (isUpdate) {
				m_inputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;
			}
			else {
				D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info;
				d3dDevice->GetRaytracingAccelerationStructurePrebuildInfo(&m_inputs, &info);
				// TLAS�̍č\�z���K�v�B
				m_topLevelASBuffers.Release();
				m_topLevelASBuffers.pScratch = CreateBuffer(d3dDevice, info.ScratchDataSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, kDefaultHeapProps);
				m_topLevelASBuffers.pResult = CreateBuffer(d3dDevice, info.ResultDataMaxSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, kDefaultHeapProps);
				m_topLevelASBuffers.pInstanceDesc = CreateBuffer(
					d3dDevice,
					sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * numInstance,
					D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ,
					kUploadHeapProps
				);
				
			}

			// �C���X�^���X�����R�s�[�B
			D3D12_RAYTRACING_INSTANCE_DESC* instanceDescs;
			m_topLevelASBuffers.pInstanceDesc->Map(0, nullptr, (void**)&instanceDescs);
			ZeroMemory(instanceDescs, sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * numInstance);

			for (int i = 0; i < numInstance; i++) {
 				instanceDescs[i].InstanceID = i;
				instanceDescs[i].InstanceContributionToHitGroupIndex = (int)eHitGroup_Num * i + eHitGroup_PBRCameraRay;
				instanceDescs[i].Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
				instanceDescs[i].AccelerationStructure = instances[i]->m_blasStructuredBuffers.pResult->GetGPUVirtualAddress();
 				memcpy(instanceDescs[i].Transform, &g_matIdentity, sizeof(instanceDescs[i].Transform));
				instanceDescs[i].InstanceMask = 0xFF;
			}

			m_topLevelASBuffers.pInstanceDesc->Unmap(0, nullptr);

			//TopLevelAS���쐬�B
			D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc = {};
			asDesc.Inputs = m_inputs;
			asDesc.Inputs.InstanceDescs = m_topLevelASBuffers.pInstanceDesc->GetGPUVirtualAddress();
			asDesc.DestAccelerationStructureData = m_topLevelASBuffers.pResult->GetGPUVirtualAddress();
			asDesc.ScratchAccelerationStructureData = m_topLevelASBuffers.pScratch->GetGPUVirtualAddress();
			if (isUpdate) {
				// �X�V�Ȃ̂Ō��f�[�^��ݒ肷��B
				asDesc.SourceAccelerationStructureData = m_topLevelASBuffers.pResult->GetGPUVirtualAddress();
			}
			else {
				// �č\�z���K�v�ł���΁A���f�[�^�͕s�v�Ȃ̂�0�����B
				asDesc.SourceAccelerationStructureData = 0;
			}
			
			// TLAS���\�z�B
			rc.BuildRaytracingAccelerationStructure(asDesc);

			//���C�g���[�V���O�A�N�Z�����[�V�����\���̃r���h�����҂��̃o���A������B
			D3D12_RESOURCE_BARRIER uavBarrier = {};
			uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
			uavBarrier.UAV.pResource = m_topLevelASBuffers.pResult;
			rc.ResourceBarrier(uavBarrier);
		}
		/// <summary>
		/// SRV�ɓo�^�B
		/// </summary>
		/// <param name="descriptorHandle"></param>
		void TLASBuffer::RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo)
		{
			auto d3dDevice = g_graphicsEngine->GetD3DDevice();
			//TLAS���f�B�X�N���v�^�q�[�v�ɓo�^�B
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			memset(&srvDesc, 0, sizeof(srvDesc));
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.RaytracingAccelerationStructure.Location = m_topLevelASBuffers.pResult->GetGPUVirtualAddress();
			d3dDevice->CreateShaderResourceView(nullptr, &srvDesc, descriptorHandle);
		}
	}
}