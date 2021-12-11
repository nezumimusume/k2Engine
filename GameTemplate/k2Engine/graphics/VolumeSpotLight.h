#pragma once

namespace nsK2Engine {
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
		/// �{�����[�����C�g�}�b�v�ɕ`��
		/// </summary>
		/// <remark>
		/// ���̊֐��̓G���W��������Ă΂�܂��B
		/// �Q�[��������Ăяo�����Ȃ��悤�ɁB
		/// </remark>
		/// <param name="rc">�����_�����O�R���e�L�X�g</param>
		void DrawToVolumeLightMap(RenderContext& rc);
	private:
		Model m_model;	// �{�����[�����C�g�`��p�̃��f���B

	};
}