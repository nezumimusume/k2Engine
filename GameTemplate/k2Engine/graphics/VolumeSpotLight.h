#pragma once

namespace nsK2Engine {
	struct SSpotLight;
	/// <summary>
	/// ���f���x�[�X�̃{�����[�����C�g
	/// </summary>
	class VolumeSpotLight {
	public:
		/// <summary>
		/// �R���X�g���N�^
		/// </summary>
		VolumeSpotLight();
		/// <summary>
		/// �f�X�g���N�^
		/// </summary>
		~VolumeSpotLight();
		/// <summary>
		/// �������B
		/// </summary>
		void Init(const SpotLight& spotLight);
		/// <summary>
		/// �X�|�b�g���C�g�̏������ƂɍX�V
		/// </summary>
		void Update();
		/// <summary>
		/// �{�����[�����C�g�̔w�ʂ�`��
		/// </summary>
		/// <remark>
		/// ���̊֐��̓G���W��������Ă΂�܂��B
		/// �Q�[��������Ăяo�����Ȃ��悤�ɁB
		/// </remark>
		/// <param name="rc">�����_�����O�R���e�L�X�g</param>
		void DrawToVolumeLightMapBack(RenderContext& rc);
		/// <summary>
		/// �{�����[�����C�g�̑O�ʂ�`��B
		/// </summary>
		/// <remark>
		/// ���̊֐��̓G���W��������Ă΂�܂��B
		/// �Q�[��������Ăяo�����Ȃ��悤�ɁB
		/// </remark>
		/// <param name="rc">�����_�����O�R���e�L�X�g</param>
		void DrawToVolumeLightMapFront(RenderContext& rc);
		/// <summary>
		/// �ŏI�`��
		/// </summary>
		void DrawFinal(RenderContext& rc);
	private:
		
		/// <summary>
		/// �ŏI�`��̃p�X�̒萔�o�b�t�@�B
		/// </summary>
		struct FinalCB {
			SSpotLight spotLight;   // �X�|�b�g���C�g�B
			Matrix mViewProjInv;   // �r���[�v���W�F�N�V�����s��̋t�s��
		};
		Model m_modelFront;	// �{�����[�����C�g�̑O�ʕ`��p�̃��f���B
		Model m_modelBack;	// �{�����[�����C�g�̔w�ʕ`��p�̃��f���B
		Sprite m_final;		// �ŏI�`��B
		FinalCB m_finalCB;	// �ŏI�`��̒萔�o�b�t�@�B
		const SpotLight* m_spotLight = nullptr;
	};
}