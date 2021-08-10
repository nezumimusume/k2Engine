#pragma once
/// <summary>
/// Ssao(�X�N���[���X�y�[�X�A���r�G���g�I�N���[�W����)�B
/// </summary>
class Ssao : public Noncopyable
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
    struct SsaoBuffer{
        Matrix view;//�r���[�s��B
        Matrix proj;//�v���W�F�N�V�����s��B
        Matrix invproj;//�t�v���W�F�N�V�����s��B
    };
    RenderTarget m_ssaoRenderTarget;
    Sprite m_ssaoSprite;					//SSAO�p�̃X�v���C�g�B
    Sprite m_finalSprite;					//�ŏI�����p�̃X�v���C�g�B
    SsaoBuffer m_cB;                    //�𑜓x��GPU�ɑ��邽�߂̒萔�o�b�t�@�\�B
};
