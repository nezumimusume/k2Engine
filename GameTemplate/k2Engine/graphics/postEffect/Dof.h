#pragma once

#include "graphics/postEffect/PostEffectComponentBase.h"

namespace nsK2Engine {
    /// <summary>
    /// ��ʊE�[�x�B
    /// </summary>
    class Dof : public PostEffectComponentBase
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
            return m_rtPhomboidBlur.GetRenderTargetTexture();
        }
        /// <summary>
        /// ��ʊE�[�x��L���ɂ���B
        /// </summary>
        void Enable()
        {
            m_isEnable = true;
        }
        /// <summary>
        /// ��ʊE�[�x�𖳌��ɂ���B
        /// </summary>
        void Disable()
        {
            m_isEnable = false;
        }
    private:
        void InitCombimeBokeImageToSprite(Sprite& combineBokeImageSprite, Texture& bokeTexture, Texture& depthTexture);
    private:
        RenderTarget m_rtVerticalBlur;		//�����u���[�������邽�߂̃����_�����O�^�[�Q�b�g
        RenderTarget m_rtDiagonalBlur;		//�Ίp���u���[�������邽�߂̃����_�����O�^�[�Q�b�g
        RenderTarget m_rtPhomboidBlur;		//�Z�p�`�u���[�������邽�߂̃����_�����O�^�[�Q�b�g
        Sprite m_vertDIagonalBlurSprite;	//�����A�Ίp���u���[�p�̃X�v���C�g
        Sprite m_phomboidBlurSprite;		//�Z�p�`�u���p�̃X�v���C�g
        Sprite m_combineBokeImageSprite;	//�{�P�摜�����C�������_�����O�^�[�Q�b�g�ɍ������邽�߂̃X�v���C�g
        bool m_isEnable = false;            //��ʊE�[�x���L���H
    };
}
