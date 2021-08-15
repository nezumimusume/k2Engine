#pragma once
#include "RaytracingInstance.h"
namespace nsK2EngineLow {
	
	namespace raytracing {
		/// <summary>
		/// BLASBuffer
		/// </summary>
		/// <remark>
		/// BLAS( Bottom level acceleration structures )�Ƃ�
		/// ���C�g�����[���h�ɓo�^����Ă���W�I���g���̃f�[�^�\���ł��B
		/// BLAS��3D���f���̃|���S����񂪓o�^����邱�ƂɂȂ�܂��B
		/// </remark>
		class BLASBuffer : public Noncopyable
		{
		public:
			/// <summary>
			/// �������B
			/// </summary>
			/// <param name="rc"></param>
			/// <param name="instance"></param>
			void Init(RenderContext& rc, const std::vector<InstancePtr>& instance);
			/// <summary>
			/// BLASBuffer�̃��X�g���擾�B
			/// </summary>
			/// <returns></returns>
			const std::vector< AccelerationStructureBuffers>& Get() const
			{
				return m_bottomLevelASBuffers;
			}
		private:

			std::vector< AccelerationStructureBuffers> m_bottomLevelASBuffers;	//BLASBuffer
		};
	}//namespace raytracing
}//namespace nsK2EngineLow 