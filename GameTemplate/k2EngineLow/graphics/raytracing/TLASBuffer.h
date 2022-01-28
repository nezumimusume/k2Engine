#pragma once
#include "RaytracingInstance.h"


namespace nsK2EngineLow {
	class RenderContext;
	namespace raytracing {
		/// <summary>
		/// TLASBuffer
		/// </summary>
		/// <remark>
		/// TLAS( Top level acceleration structures )�Ƃ�
		/// ���C�g�����[���h�ɓo�^����Ă���C���X�^���X�̃f�[�^�\���ł��B
		/// �C���X�^���X�̎g�p����W�I���g���A�V�F�[�_�[ID�A���[���h�s��Ȃǂ�
		/// �f�[�^��ێ����܂��B
		/// </remark>
		class TLASBuffer : public IShaderResource
		{
		public:
			/// <summary>
			/// TLAS���\�z�B
			/// </summary>
			/// <param name="rc"></param>
			/// <param name="instances"></param>
			void Build(
				RenderContext& rc,
				const std::vector<InstancePtr>& instances,
				bool isUpdate);
			/// <summary>
			/// SRV�ɓo�^�B
			/// </summary>
			/// <param name="descriptorHandle"></param>
			void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo) override;
			/// <summary>
			/// VRAM��̉��z�A�h���X���擾�B
			/// </summary>
			/// <returns></returns>
			D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const
			{
				return m_topLevelASBuffers.pResult->GetGPUVirtualAddress();
			}
		private:
			D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS m_inputs = {};
			AccelerationStructureBuffers m_topLevelASBuffers;
		};
	}//namespace raytracing
}