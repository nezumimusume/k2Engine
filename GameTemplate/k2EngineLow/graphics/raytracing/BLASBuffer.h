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
			/// BLAS�̃r���h�B
			/// </summary>
			/// <remark>
			/// BLAS�̍č\�z���s�v�ł���΁A�X�V�݂̂��s���܂��B
			/// </remark>
			/// <param name="rc">�����_�����O�R���e�L�X�g</param>
			/// <param name="instances">�C���X�^���X�̔z��</param>
			void Build(RenderContext& rc, const std::vector<InstancePtr>& instances, bool isUpdate);
			
		private:

		};
	}//namespace raytracing
}//namespace nsK2EngineLow 