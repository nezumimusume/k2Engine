#pragma once
/// <summary>
/// FXAA�B
/// </summary>
class Fxaa : public Noncopyable
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

    struct FaxxBuffer
    {
        float bufferW;
        float bufferH;
    }; 

    Sprite m_finalSprite;					//�ŏI�����p�̃X�v���C�g
    FaxxBuffer m_cB;                    //�𑜓x��GPU�ɑ��邽�߂̒萔�o�b�t�@�\�B
};