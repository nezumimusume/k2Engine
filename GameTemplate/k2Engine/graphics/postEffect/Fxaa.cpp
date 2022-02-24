#include "k2EnginePreCompile.h"
#include "Fxaa.h"

namespace nsK2Engine {
    void Fxaa::OnInit(
        RenderTarget& mainRenderTarget,
        RenderTarget& zprepassRenderTarget,
        RenderTarget& normalRenderTarget,
        RenderTarget& metallicSmoothRenderTarget,
        RenderTarget& albedoRenderTarget
    ) {
        // �ŏI�����p�̃X�v���C�g������������
        SpriteInitData spriteInitData;
        spriteInitData.m_textures[0] = &mainRenderTarget.GetRenderTargetTexture();
        // �𑜓x��mainRenderTarget�̕��ƍ���
        spriteInitData.m_width = mainRenderTarget.GetWidth();
        spriteInitData.m_height = mainRenderTarget.GetHeight();
        // 2D�p�̃V�F�[�_�[���g�p����
        spriteInitData.m_fxFilePath = "Assets/shader/postEffect/fxaa.fx";
        spriteInitData.m_vsEntryPointFunc = "VSMain";
        spriteInitData.m_psEntryPoinFunc = "PSMain";

        spriteInitData.m_alphaBlendMode = AlphaBlendMode_None;

        //�𑜓x��GPU�ɑ��邽�߂̒萔�o�b�t�@��ݒ肷��B
        spriteInitData.m_expandConstantBuffer = (void*)&m_cB;
        spriteInitData.m_expandConstantBufferSize = sizeof(FaxxBuffer) +
            (16 - (sizeof(FaxxBuffer) % 16));
        m_finalSprite.Init(spriteInitData);

        m_fxaaRt.Create(
            mainRenderTarget.GetWidth(),
            mainRenderTarget.GetHeight(),
            1,
            1,
            DXGI_FORMAT_R8G8B8A8_UNORM, // HDR�n�̃G�t�F�N�g�͏I�����Ă���̂ŁA8bit�����o�b�t�@�łn�j�B
            DXGI_FORMAT_UNKNOWN
        );


    }

    void Fxaa::OnRender(RenderContext& rc, RenderTarget& mainRenderTarget)
    {
        g_graphicsEngine->BeginGPUEvent("FXAA");
        // �����_�����O�^�[�Q�b�g�Ƃ��ė��p�ł���܂ő҂�
        rc.WaitUntilToPossibleSetRenderTarget(m_fxaaRt);
        // �����_�����O�^�[�Q�b�g��ݒ�
        rc.SetRenderTargetAndViewport(m_fxaaRt);
        m_cB.bufferW = static_cast<float>(g_graphicsEngine->GetFrameBufferWidth());
        m_cB.bufferH = static_cast<float>(g_graphicsEngine->GetFrameBufferHeight());
        //�`��B
        m_finalSprite.Draw(rc);
        // �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
        //���C�������_�\�^�[�Q�b�g��RENDERTARGET����PRESENT�ցB
        rc.WaitUntilFinishDrawingToRenderTarget(m_fxaaRt);
        g_graphicsEngine->EndGPUEvent();
    }
}