#pragma once

#include "graphics/PostEffectBase.h"

/// <summary>
/// �X�N���[���X�y�[�X���t���N�V����
/// </summary>
class Ssr : public PostEffectBase {
public:
	/// <summary>
	/// �������B
	/// </summary>
	void OnInit(
		RenderTarget& mainRenderTarget, 
		RenderTarget& depthRenderTarget,
		RenderTarget& normalRenderTarget,
		RenderTarget& metallicSmoothRenderTarget,
		RenderTarget& albedoRenderTarget
	) override;
	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="rc"></param>
	void OnRender(RenderContext& rc, RenderTarget& mainRenderTarget) override;
	/// <summary>
	/// �|�X�g�̌��ʂ̉摜�����C�������_�����O�^�[�Q�b�g�ɃR�s�[����H
	/// </summary>
	/// <returns></returns>
	bool IsCopyResultTextureToMainRenderTarget() const override
	{
		return true;
	}
	/// <summary>
    /// �|�X�g�G�t�F�N�g�����s�������ʂƂȂ�e�N�X�`�����擾�B
    /// </summary>
    /// <returns></returns>
	Texture& GetResultTexture() override
	{
		return m_finalRt.GetRenderTargetTexture();
	}
	/// <summary>
	/// �L���ɂ���B
	/// </summary>
	void Enable()
	{
		m_isEnable = true;
	}
	/// <summary>
	/// �����ɂ���B
	/// </summary>
	void Disable()
	{
		m_isEnable = false;
	}
private:
	struct SsrCb {
		Matrix mViewProjInv;				// �r���[�v���W�F�N�V�����s��̋t�s��B
		Matrix mViewProj;					// �r���[�v���W�F�N�V�����s��B
		Vector3 cameraPosInWorld;			// ���[���h���W�n�ł̃J�����̎��_�B
	};
	RenderTarget m_reflectionRt;			// �f�荞�݂����������ނ��߂̃����_�����O�^�[�Q�b�g�B
	RenderTarget m_finalRt;					// �ŏI�����X�v���C�g
	GaussianBlur m_blur;
	SsrCb m_ssrCb;
	Sprite m_sprite;
	Sprite m_finalSprite;					// �ŏI�����X�v���C�g�B
	bool m_isEnable = true;					// SSR�̗L���A�����B
};