#pragma once

#include "DescriptorHeap.h"
#include "RaytracingInstance.h"
#include "TLASBuffer.h"
#include "BLASBuffer.h"
#include "RaytracingPSO.h"
#include "RaytracingShaderTable.h"
#include "RaytracingDescriptorHeaps.h"

namespace nsK2EngineLow {
	class Model;
	namespace raytracing {


		class Engine : public Noncopyable
		{
		public:
			/// <summary>
			/// ���C�g���[�V���O���f�B�X�p�b�`�B
			/// </summary>
			/// <param name="rc">�����_�����O�R���e�L�X�g</param>
			void Dispatch(RenderContext& rc);
			/// <summary>
			/// �W�I���g����o�^�B
			/// </summary>
			/// <param name="model">���f��</param>
			void RegistGeometry(Model& model)
			{
				//���C�g�����[���h�ɃW�I���g����o�^�B
				m_world.RegistGeometry(model);
			}
			/// <summary>
			/// �W�I���g���̓o�^���m��B
			/// </summary>
			void CommitRegistGeometry(RenderContext& rc);
		private:
			/// <summary>
			/// �V�F�[�_�[�e�[�u�����쐬�B
			/// </summary>
			/// <param name="rc"></param>
			void CreateShaderTable(RenderContext& rc);

			/// <summary>
			/// �V�F�[�_�[���\�[�X���쐬�B
			/// </summary>
			void CreateShaderResources();

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
			ConstantBuffer m_rayGenerationCB;			//���C�W�F�l���[�V�����̒萔�o�b�t�@�B
			World m_world;								//���C�g�����[���h�B
			PSO m_pipelineStateObject;					//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
			ShaderTable m_shaderTable;					//�V�F�[�_�[�e�[�u���B
			DescriptorHeaps m_descriptorHeaps;			//���C�g���Ŏg�p����f�B�X�N���v�^�q�[�v�̊Ǘ��ҁB
			GPUBuffer m_outputResource;					//���C�g���[�X�̌��ʂ̏o�͐�B
		};
	}//namespace raytracing
}//namespace nsK2EngineLow 