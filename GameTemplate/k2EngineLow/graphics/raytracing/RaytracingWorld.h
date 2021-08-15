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
			/// <param name="model">���f��</param>
			void RegistGeometry(Model& model);
			/// <summary>
			/// �W�I���g���̓o�^���m��B
			/// </summary>
			void CommitRegistGeometry(RenderContext& rc);
			/// <summary>
			/// ���C�g�����[���h�̃C���X�^���X�ɑ΂��ăN�G�����s���B
			/// </summary>
			/// <param name="queryFunc"></param>
			void QueryInstances(std::function<void(Instance&)> queryFunc) const
			{
				for (auto& instance : m_instances) {
					queryFunc(*instance);
				}
			}
			/// <summary>
			/// �C���X�^���X�̐����擾�B
			/// </summary>
			/// <returns></returns>
			int GetNumInstance() const
			{
				return static_cast<int>(m_instances.size());
			}
			/// <summary>
			/// BLAS�o�b�t�@�[���擾�B
			/// </summary>
			/// <returns></returns>
			const BLASBuffer& GetBLASBuffer()
			{
				return m_blasBuffer;
			}
			/// <summary>
			/// TLAS�o�b�t�@�[���擾�B
			/// </summary>
			/// <returns></returns>
			TLASBuffer& GetTLASBuffer()
			{
				return m_topLevelASBuffers;
			}
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
			Camera m_camera;									//���C�g�����[���h�̃J�����B
			std::vector<InstancePtr> m_instances;		//���C�g�����[���h�̃C���X�^���̔z��B
			BLASBuffer m_blasBuffer;							//BLAS
			TLASBuffer m_topLevelASBuffers;						//TLAS
		};
	}//namespace raytracing
}