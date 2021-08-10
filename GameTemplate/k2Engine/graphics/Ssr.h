#pragma once


/// <summary>
/// �X�N���[���X�y�[�X���t���N�V����
/// </summary>
class Ssr {
public:
	/// <summary>
	/// �������B
	/// </summary>
	void Init(
		RenderTarget& mainRenderTarget, 
		RenderTarget& depthRenderTarget,
		RenderTarget& normalRenderTarget,
		RenderTarget& metallicSmoothRenderTarget,
		RenderTarget& albedoRenderTarget
	);
	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="rc"></param>
	void Render(RenderContext& rc, RenderTarget& mainRenderTarget);
private:
	struct SsrCb {
		Matrix mViewProjInv;				//�r���[�v���W�F�N�V�����s��̋t�s��B
		Matrix mViewProj;					//�r���[�v���W�F�N�V�����s��B
		Vector3 cameraPosInWorld;			//���[���h���W�n�ł̃J�����̎��_�B
	};
	RenderTarget m_reflectionRt;				//�f�荞�݂����������ނ��߂̃����_�����O�^�[�Q�b�g�B
	GaussianBlur m_blur;
	SsrCb m_ssrCb;
	Sprite m_sprite;
	Sprite m_finalSprite;						//�ŏI�����X�v���C�g�B
};