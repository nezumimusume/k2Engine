#pragma once

namespace nsK2Engine {
	/// <summary>
	/// ���C�g�J�����O�B
	/// </summary>
	class LightCulling : public Noncopyable{
	public:
		/// <summary>
		/// �������B
		/// </summary>
		/// <param name="depthTexture">�V�[���̐[�x�l���������܂��e�N�X�`��</param>
		/// <param name="lightCb">���C�g�f�[�^�̒萔�o�b�t�@�B</param>
		/// <param name="pointLightNoListInTileUAV">�^�C�����Ƃ̃|�C���g���C�g�̃��X�g���o�͂��邽�߂�UAV�B</param>
		/// <param name="spotLightNoListInTileUAV">�^�C�����Ƃ̃X�|�b�g���C�g�̃��X�g���o�͂��邽�߂�UAV�B</param>
		void Init(
			Texture& depthTexture, 
			ConstantBuffer& lightCB, 
			RWStructuredBuffer& pointLightNoListInTileUAV,
			RWStructuredBuffer& spotLightNoListInTileUAV
		);

		/// <summary>
		/// ���C�g�J�����O�����s�B
		/// </summary>
		/// <param name="rc">�����_�����O�R���e�L�X�g</param>
		void Execute(RenderContext& rc);
	private:
		// ���C�g�J�����O�Ŏg�p����J�������
		struct CameraData
		{
			Matrix mProj;           // �v���W�F�N�V�����s��
			Matrix mProjInv;        // �v���W�F�N�V�����s��̋t�s��
			Matrix mCameraRot;      // �J�����̉�]�s��
			Vector4 screenParam;    // �X�N���[�����
		};
		RootSignature m_rootSignature;					// ���[�g�V�O�l�`���B
		ConstantBuffer m_cameraDataCB;					// �J�����f�[�^�̒萔�o�b�t�@
		PipelineState m_pipelineState;					// �p�C�v���C���X�e�[�g�B
		Shader m_shader;								// ���C�g�J�����O�V�F�[�_�[�B
		DescriptorHeap m_descriptorHeap;				// �f�B�X�N���v�^�q�[�v�B
		RWStructuredBuffer* m_pointLightNoListInTileUAV = nullptr;	// �^�C�����Ƃ̃|�C���g���C�g�̃��X�g��UAV�B
		RWStructuredBuffer* m_spotLightNoListInTileUAV = nullptr;	// �^�C�����Ƃ̃X�|�b�g���C�g�̃��X�g��UAV�B
	};
}