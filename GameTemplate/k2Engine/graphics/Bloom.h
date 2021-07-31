#pragma once

/// <summary>
/// �u���[���B
/// </summary>
class Bloom : public Noncopyable
{
public:
    /// <summary>
    /// �������B
    /// </summary>
    /// <param name="mainRenderTarget">���C�������_�[�^�[�Q�b�g�B</param>
    void Init(RenderTarget& mainRenderTarget);
    /// <summary>
    /// �`��B
    /// </summary>
    /// <param name="rc">�����_�\�R���e�L�X�g�B</param>
    /// <param name="mainRenderTarget">���C�������_�\�^�[�Q�b�g�B</param>
    void Render(RenderContext& rc, RenderTarget& mainRenderTarget);
private:
    RenderTarget m_luminanceRenderTarget;	//�P�x���o�p�̃����_�����O�^�[�Q�b�g
    Sprite m_luminanceSprite;				//�P�x���o�p�̃X�v���C�g
    GaussianBlur m_gaussianBlur[4];			//�K�E�V�A���u���[
    Sprite m_finalSprite;					//�ŏI�����p�̃X�v���C�g
};
