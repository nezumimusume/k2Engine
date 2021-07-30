#include "k2EnginePreCompile.h"
#include "Fxaa.h"

void Fxaa::Init(RenderTarget& mainRenderTarget)
{
    // �ŏI�����p�̃X�v���C�g������������
    SpriteInitData spriteInitData;
    spriteInitData.m_textures[0] = &mainRenderTarget.GetRenderTargetTexture();
    // �𑜓x��mainRenderTarget�̕��ƍ���
    spriteInitData.m_width = mainRenderTarget.GetWidth();
    spriteInitData.m_height = mainRenderTarget.GetHeight();
    // 2D�p�̃V�F�[�_�[���g�p����
    spriteInitData.m_fxFilePath = "Assets/shader/fxaa.fx";
    spriteInitData.m_vsEntryPointFunc = "VSMain";
    spriteInitData.m_psEntryPoinFunc = "PSMain";

    spriteInitData.m_alphaBlendMode = AlphaBlendMode_None;
    spriteInitData.m_colorBufferFormat[0] = mainRenderTarget.GetColorBufferFormat();

    //�𑜓x��GPU�ɑ��邽�߂̒萔�o�b�t�@��ݒ肷��B
    spriteInitData.m_expandConstantBuffer = (void*)&m_cB;
    spriteInitData.m_expandConstantBufferSize = sizeof(FaxxBuffer) + 
        (16 - (sizeof(FaxxBuffer) % 16));
    m_finalSprite.Init(spriteInitData);
}

void Fxaa::Render(RenderContext& rc, RenderTarget& mainRenderTarget)
{
    // �����_�����O�^�[�Q�b�g�Ƃ��ė��p�ł���܂ő҂�
    rc.WaitUntilToPossibleSetRenderTarget(mainRenderTarget);
    // �����_�����O�^�[�Q�b�g��ݒ�
    rc.SetRenderTargetAndViewport(mainRenderTarget);
    m_cB.bufferW = static_cast<float>(g_graphicsEngine->GetFrameBufferWidth());
    m_cB.bufferH = static_cast<float>(g_graphicsEngine->GetFrameBufferHeight());
    //�`��B
    m_finalSprite.Draw(rc);
    // �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
    //���C�������_�\�^�[�Q�b�g��RENDERTARGET����PRESENT�ցB
    rc.WaitUntilFinishDrawingToRenderTarget(mainRenderTarget);
}
