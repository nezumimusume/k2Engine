#pragma once

#include "graphics/postEffect/PostEffectComponentBase.h"

namespace nsK2Engine {
    /// <summary>
    /// �u���[���B
    /// </summary>
    class Bloom : public PostEffectComponentBase
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
            return m_luminanceRenderTarget.GetRenderTargetTexture();
        }
        /// <summary>
        /// �u���[������������臒l��ݒ�B
        /// </summary>
        /// <param name="value"></param>
        void SetThreshold(float value)
        {
            m_samplingLuminanceCB1.threshold = value;
        }
        /// <summary>
        /// �u���[������������臒l���擾�B
        /// </summary>
        /// <returns></returns>
        float GetThreshold() const
        {
            return m_samplingLuminanceCB1.threshold;
        }
    private:
        /// <summary>
        /// �P�x���o�p�X�̒萔�o�b�t�@�P�ɑ�����e�B
        /// </summary>
        struct SSamplingLuminanceCB1 {
            int isTonemap;      // �g�[���}�b�v���L���H
            float middlegray;   // �~�h���O���[�B
            float threshold = 1.0f;
        };
        RenderTarget m_luminanceRenderTarget;	//�P�x���o�p�̃����_�����O�^�[�Q�b�g
        Sprite m_luminanceSprite;				//�P�x���o�p�̃X�v���C�g
        GaussianBlur m_gaussianBlur[4];			//�K�E�V�A���u���[
        Sprite m_finalSprite;					//�ŏI�����p�̃X�v���C�g
        SSamplingLuminanceCB1 m_samplingLuminanceCB1;
    };
}