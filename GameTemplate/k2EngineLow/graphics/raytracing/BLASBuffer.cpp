#include "k2EngineLowPreCompile.h"
#include "BLASBuffer.h"

namespace nsK2EngineLow {
	namespace raytracing {
		 
		void BLASBuffer::Build(RenderContext& rc, const std::vector<InstancePtr>& instances, bool isUpdate)
		{
			for (auto& instance : instances) {
				Matrix mWorld = instance->m_model->GetWorldMatrix();
				instance->geometoryDesc.Triangles.IndexCount = instance->m_originalIndexBuffer->GetCount();
				mWorld.Transpose();
				if (instance->m_worldMatrixCB.IsValid()) {
					instance->m_worldMatrixCB.CopyToVRAM(mWorld);
					instance->geometoryDesc.Triangles.Transform3x4 = instance->m_worldMatrixCB.GetGPUVirtualAddress();
				}
				D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
				inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
				if (isUpdate) {
					// �X�V�݂̂�OK�B
					inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE | D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;				
				}
				else {
					// BLAS�̍č\�z���K�v�B
					inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
					
				}
				inputs.NumDescs = 1;
				inputs.pGeometryDescs = &instance->geometoryDesc;
				inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;

				auto d3dDevice = g_graphicsEngine->GetD3DDevice();
				D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info;
				d3dDevice->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

				if (instance->m_blasStructuredBuffers.pScratch == nullptr) {
					instance->m_blasStructuredBuffers.pScratch = CreateBuffer(
						d3dDevice,
						info.ScratchDataSizeInBytes,
						D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
						D3D12_RESOURCE_STATE_COMMON,
						kDefaultHeapProps);

					instance->m_blasStructuredBuffers.pResult = CreateBuffer(
						d3dDevice,
						info.ResultDataMaxSizeInBytes,
						D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
						D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
						kDefaultHeapProps);
				}
				// Create the bottom-level AS
				D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc = {};
				asDesc.Inputs = inputs;
				asDesc.DestAccelerationStructureData = instance->m_blasStructuredBuffers.pResult->GetGPUVirtualAddress();
				asDesc.ScratchAccelerationStructureData = instance->m_blasStructuredBuffers.pScratch->GetGPUVirtualAddress();
				if (isUpdate) {
					// �X�V�ł���΁A���f�[�^���g�p����B
					asDesc.SourceAccelerationStructureData = instance->m_blasStructuredBuffers.pResult->GetGPUVirtualAddress();
					
				}
				else {
					// �č\�z���K�v�Ȃ̂Ō��f�[�^�͂���Ȃ��̂�nullptr��ݒ肷��B
					asDesc.SourceAccelerationStructureData = 0;
				}
				rc.BuildRaytracingAccelerationStructure(asDesc);

				//���C�g���[�V���O�A�N�Z�����[�V�����\���̃r���h�����҂��̃o���A������B
				D3D12_RESOURCE_BARRIER uavBarrier = {};
				uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
				uavBarrier.UAV.pResource = instance->m_blasStructuredBuffers.pResult;
				rc.ResourceBarrier(uavBarrier);
			}
		}
	}//namespace raytracing
}// namespace nsK2EngineLow