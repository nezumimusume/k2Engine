#pragma once

#include "graphics/postEffect/PostEffectComponentBase.h"

namespace nsK2Engine {
    /// <summary>
    /// FXAA�B
    /// </summary>
    class Fxaa : public PostEffectComponentBase
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
        /// �|�X�g�G�t�F�N�g�����s�������ʂƂȂ�e�N�X�`�����擾�B
        /// </summary>
        /// <returns></returns>
        Texture& GetResultTexture() override
        {
            return m_fxaaRt.GetRenderTargetTexture();
        }
        /// <summary>
        /// �|�X�g�̌��ʂ̉摜�����C�������_�����O�^�[�Q�b�g�ɃR�s�[����H
        /// </summary>
        /// <returns></returns>
        bool IsCopyResultTextureToMainRenderTarget() const override
        {
            return true;
        }
    private:

        struct FaxxBuffer
        {
            float bufferW;
            float bufferH;
        };
        RenderTarget m_fxaaRt;  // FXAA���s�������_�����O�^�[�Q�b�g�B
        Sprite m_finalSprite;	// �ŏI�����p�̃X�v���C�g
        Sprite m_copySprite;    //�@
        FaxxBuffer m_cB;        // �𑜓x��GPU�ɑ��邽�߂̒萔�o�b�t�@�\�B
    };
}