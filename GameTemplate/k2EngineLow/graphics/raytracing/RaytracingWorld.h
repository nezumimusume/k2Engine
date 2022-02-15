#pragma once

#include "graphics/DescriptorHeap.h"
#include "RaytracingInstance.h"
#include "TLASBuffer.h"
#include "BLASBuffer.h"



namespace nsK2EngineLow {
	class Model;
	class RenderContext;
	namespace raytracing {

		/// <summary>
		/// ���C�g�����[���h
		/// </summary>
		class World : public Noncopyable
		{
		public:
			/// <summary>
			/// �W�I���g����o�^�B
			/// </summary>
			/// <param name="model">�W�I���g���̌��ƂȂ郂�f��</param>
			void RegistGeometry(Model& model);
			/// <summary>
			/// �W�I���g�����폜
			/// </summary>
			/// <param name="model">�W�I���g���̌��ƂȂ������f��</param>
			void RemoveGeometry(Model& model);
			/// <summary>
			/// �W�I���g���̓o�^���m��B
			/// </summary>
			void CommitRegistGeometry(RenderContext& rc);
			
			/// <summary>
			/// ���C�g�����[���h���\�z�B
			/// </summary>
			void Build( RenderContext& rc );

			/// <summary>
			/// ���C�g�����[���h�̃C���X�^���X�ɑ΂��ăN�G�����s���B
			/// </summary>
			/// <param name="bufferNo">�o�b�t�@�̔ԍ�</param>
			/// <param name="queryFunc">�N�G���֐�</param>
			void QueryInstances(int bufferNo, std::function<void(Instance&)> queryFunc) const;
			
			/// <summary>
			/// �C���X�^���X�̐����擾�B
			/// </summary>
			/// <returns></returns>
			int GetNumInstance() const;
			
			/// <summary>
			/// BLAS�o�b�t�@�[���擾�B
			/// </summary>
			/// <returns></returns>
			const BLASBuffer& GetBLASBuffer(int bufferNo);
			
			/// <summary>
			/// TLAS�o�b�t�@�[���擾�B
			/// </summary>
			/// <returns></returns>
			TLASBuffer& GetTLASBuffer(int bufferNo);
			
		private:
			/// <summary>
			/// ���C�g���̃C���X�^���X���쐬�B
			/// </summary>
			void CreateRaytracingInstance(Model& model, int bufferNo);
		private:
			/// <summary>
			/// �J����
			/// </summary>
			struct Camera {
				Matrix mRot;	//��]�s��
				Vector3 pos;	//���_�B
				float aspect;	//�A�X�y�N�g��B
				float fFar;		//�����ʁB
				float fNear;	//�ߕ��ʁB
			};
			
			Camera m_camera;								// ���C�g�����[���h�̃J�����B
			std::vector<InstancePtr> m_instances[2];		// ���C�g�����[���h�̃C���X�^���̔z��B
			BLASBuffer m_blasBuffer[2];						// BLAS
			TLASBuffer m_topLevelASBuffers[2];				// TLAS
		};
	}//namespace raytracing
}
