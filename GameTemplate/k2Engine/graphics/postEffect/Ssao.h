#pragma once

#include "graphics/postEffect/PostEffectComponentBase.h"

namespace nsK2Engine {
    /// <summary>
    /// Ssao(�X�N���[���X�y�[�X�A���r�G���g�I�N���[�W����)�B
    /// </summary>
    class Ssao : public PostEffectComponentBase
    {
    public:
        /// <summary>
        /// �������B
        /// </summary>
        /// <param name="mainRenderTarget">���C�������_�[�^�[�Q�b�g�B</param>
        void OnInit(
            RenderTarget& mainRenderTarget,
            RenderTarget& zprepassRenderTarget,
            RenderTarget& normalRenderTarget,
            RenderTarget& metallicSmoothRenderTarget,
            RenderTarget& albedoRenderTarget) override;
        /// <summary>
        /// �`��B
        /// </summary>
        /// <param name="rc">�����_�\�R���e�L�X�g�B</param>
        /// <param name="mainRenderTarget">���C�������_�\�^�[�Q�b�g�B</param>
        void OnRender(RenderContext& rc, RenderTarget& mainRenderTarget) override;
        /// <summary>
        /// �|�X�g�̌��ʂ̉摜�����C�������_�����O�^�[�Q�b�g�ɃR�s�[����H
        /// </summary>
        /// <returns></returns>
        bool IsCopyResultTextureToMainRenderTarget() const override
        {
            return false;
        }
        /// <summary>
        /// �|�X�g�G�t�F�N�g�����s�������ʂƂȂ�e�N�X�`�����擾�B
        /// </summary>
        /// <returns></returns>
        Texture& GetResultTexture() override
        {
            return m_ssaoRenderTarget.GetRenderTargetTexture();
        }
    private:
        struct SsaoBuffer {
            Matrix view;//�r���[�s��B
            Matrix proj;//�v���W�F�N�V�����s��B
            Matrix invproj;//�t�v���W�F�N�V�����s��B
        };
        RenderTarget m_ssaoRenderTarget;
        Sprite m_ssaoSprite;					//SSAO�p�̃X�v���C�g�B
        Sprite m_finalSprite;					//�ŏI�����p�̃X�v���C�g�B
        SsaoBuffer m_cB;                    //�𑜓x��GPU�ɑ��邽�߂̒萔�o�b�t�@�\�B
    };
}