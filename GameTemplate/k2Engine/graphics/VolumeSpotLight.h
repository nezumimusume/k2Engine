#pragma once

namespace nsK2Engine {
	struct SpotLight;
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
		void Init();
		/// <summary>
		/// �X�|�b�g���C�g�̏������ƂɍX�V
		/// </summary>
		/// <param name="spotLight">�X�|�b�g���C�g</param>
		void Update(const SpotLight& spotLight);
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
	private:
		/// <summary>
		/// GPU�ɑ���X�|�b�g���C�g���
		/// </summary>
		struct SpotLightInfo {
			int no;	// �X�|�b�g���C�g�̔ԍ��B
		};
		SpotLightInfo m_spotLightInfo;	// GPU�ɑ���X�|�b�g���C�g���B
		Model m_modelFront;	// �{�����[�����C�g�̑O�ʕ`��p�̃��f���B
		Model m_modelBack;	// �{�����[�����C�g�̔w�ʕ`��p�̃��f���B
	};
}