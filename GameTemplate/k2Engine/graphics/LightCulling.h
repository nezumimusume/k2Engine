#pragma once

/// <summary>
/// ���C�g�J�����O�B
/// </summary>
class LightCulling {
public:
	/// <summary>
	/// �������B
	/// </summary>
	/// <param name="depthTexture">�V�[���̐[�x�l���������܂��e�N�X�`��</param>
	/// /// <param name="lightCb">���C�g�f�[�^�̒萔�o�b�t�@�B</param>
	void Init(Texture& depthTexture, ConstantBuffer& lightCB);
private:
	// ���C�g�J�����O�Ŏg�p����J�������
	struct CameraData
	{
		Matrix mProj;           // �v���W�F�N�V�����s��
		Matrix mProjInv;        // �v���W�F�N�V�����s��̋t�s��
		Matrix mCameraRot;      // �J�����̉�]�s��
		Vector4 screenParam;    // �X�N���[�����
	};
	RWStructuredBuffer m_pointLightNoListInTileUAV;	// �^�C�����Ƃ̃|�C���g���C�g�̔ԍ��̃��X�g�BGPU���B
	ConstantBuffer m_cameraDataCB;					// �J�����f�[�^�̒萔�o�b�t�@
	
	Shader m_shader;								// ���C�g�J�����O�V�F�[�_�[�B
	DescriptorHeap m_descriptroHeap;				// �f�B�X�N���v�^�q�[�v�B

};