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

		/// <summary>
		/// ���������
		/// </summary>
		struct InitData {
			void*	m_expandShaderResource;			// �g���V�F�[�_�[���\�[�X�̔z��B
			int		m_expandShaderResourceSize;		// �g���V�F�[�_�[���\�[�X�̃T�C�Y�̔z��B
			DXGI_FORMAT m_outputColorBufferFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;	// ���C�g���̌��ʂ��o�͂���J���[�o�b�t�@�̃t�H�[�}�b�g�B
		};
		/// <summary>
		/// �g���V�F�[�_�[���\�[�X
		/// </summary>
		struct ExpanadSRV {
			void Init(void* srcData, int srcDataSize)
			{
				m_srcData = srcData;
				m_srcDataSize = srcDataSize;
				m_structuredBuffer.Init(srcDataSize, 1, srcData, false);
			}
			void* m_srcData = nullptr;				// �\�[�X�f�[�^�B
			int m_srcDataSize = 0;					// �\�[�X�f�[�^�̃T�C�Y�B
			StructuredBuffer m_structuredBuffer;	// �X�g���N�`���[�h�o�b�t�@�B
		};
		using ExpanadSRVPtr = std::unique_ptr< ExpanadSRV>;

		class Engine : public Noncopyable
		{
		public:
			
			/// <summary>
			/// �������B
			/// </summary>
			/// <param name="initData">�������f�[�^</param>
			void Init(const InitData& initData);
			/// <summary>
			/// ���C�g���[�V���O���f�B�X�p�b�`�B
			/// </summary>
			/// <param name="rc">�����_�����O�R���e�L�X�g</param>
			void Dispatch(RenderContext& rc);
			/// <summary>
			/// ���C�g�����[���h�̍č\�z���N�G�X�g
			/// </summary>
			void RequestRebuildRaytracingWorld()
			{
				m_isDirty = true;
			}
			/// <summary>
			/// �W�I���g����o�^�B
			/// </summary>
			/// <param name="model">�W�I���g���̌��ƂȂ郂�f��</param>
			void RegistGeometry(Model& model)
			{
				//���C�g�����[���h�ɃW�I���g����o�^�B
				m_world.RegistGeometry(model);
				RequestRebuildRaytracingWorld();
			}
			/// <summary>
			/// �W�I���g�����폜
			/// </summary>
			/// <param name="model">�W�I���g���̌��ƂȂ������f��</param>
			void RemoveGeometry(Model& model)
			{
				m_world.RemoveGeometry(model);
				RequestRebuildRaytracingWorld();
			}
			/// <summary>
			/// �X�J�C�L���[�u�{�b�N�X��ݒ�B
			/// </summary>
			/// <param name="skycubeBox"></param>
			void SetSkyCubeBox(Texture& skycubeBox)
			{
				if (skycubeBox.Get() != nullptr) {
					m_skycubeBox.IniteFromTexture(skycubeBox);
					RequestRebuildRaytracingWorld();
				}
			}
			/// <summary>
			/// ���C�g���̌��ʂ̏o�͐�ƂȂ�e�N�X�`�����擾�B
			/// </summary>
			/// <returns></returns>
			Texture& GetOutputTexture()
			{
				return m_outputTexture;
			}
		private:
			/// <summary>
			/// �W�I���g���̓o�^���m��B
			/// </summary>
			void CommitRegistGeometry(RenderContext& rc);
			/// <summary>
			/// ���C�g���[�V���O�Ŏg�p����J�����\���́B
			/// ���̒��g��ύX������AAssets/shader/raytracing.fx���ύX����悤�ɁB
			/// </summary>
			struct Camera {
				Matrix mViewProjInv;	// �r���[�v���W�F�N�V�����s��̋t�s��
				Vector3 pos;			// ���_�B
				float aspect;			// �A�X�y�N�g��B
				float fFar;				// �����ʁB
				float fNear;			// �ߕ��ʁB
				float pad[2];			// �p�f�B���O�B
			};
			ExpanadSRVPtr m_expandSRV[2];						// �g���V�F�[�_�[���\�[�X�r���[�B
			ConstantBuffer m_rayGenerationCB[2];				// ���C�W�F�l���[�V�����̒萔�o�b�t�@�B
			World m_world;										// ���C�g�����[���h�B
			PSO m_pipelineStateObject[2];						// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
			ShaderTable m_shaderTable[2];						// �V�F�[�_�[�e�[�u���B
			DescriptorHeaps m_descriptorHeaps[2];				// ���C�g���Ŏg�p����f�B�X�N���v�^�q�[�v�̊Ǘ��ҁB
			GPUBuffer m_outputResource;							// ���C�g���[�X�̌��ʂ̏o�͐�B
			Texture m_outputTexture;							// ���C�g���[�X�̌��ʂ̏o�͐�(�e�N�X�`��)
			Texture m_skycubeBox;								// �X�J�C�L���[�u�{�b�N�X�B
			bool m_isDirty = false;								// �_�[�e�B�t���O�B
		};
	}//namespace raytracing
}//namespace nsK2EngineLow 

