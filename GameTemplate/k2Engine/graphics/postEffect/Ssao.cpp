#include "k2EnginePreCompile.h"
#include "Ssao.h"
#include "Graphics/RenderingEngine.h"

namespace nsK2Engine {
    void Ssao::OnInit(
        RenderTarget& mainRenderTarget,
        RenderTarget& zprepassRenderTarget,
        RenderTarget& normalRenderTarget,
        RenderTarget& metallicSmoothRenderTarget,
        RenderTarget& albedoRenderTarget
    )
    {
        float color[4] =
        {
            0.0f,0.0f,0.0f,0.0f
        };

        m_ssaoRenderTarget.Create(
            mainRenderTarget.GetWidth(),
            mainRenderTarget.GetHeight(),
            1,
            1,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            DXGI_FORMAT_UNKNOWN,
            color
        );

        // �ŏI�����p�̃X�v���C�g������������
        SpriteInitData spriteInitData;
        spriteInitData.m_textures[0] = &g_renderingEngine->GetZPrepassDepthTexture();
        spriteInitData.m_textures[1] = &g_renderingEngine->GetGBufferNormalTexture();
        //todo ���[���h���W��GBuffer�͔p�~�B�A���x�h�[�x�e�N�X�`���̐[�x�l�𗘗p���āA���[���h���W�𕜌�����悤�ɁB
        //spriteInitData.m_textures[2] = &g_renderingEngine->GetGBufferWorldTexture();
        // �𑜓x��mainRenderTarget�̕��ƍ���
        spriteInitData.m_width = mainRenderTarget.GetWidth();
        spriteInitData.m_height = mainRenderTarget.GetHeight();
        // 2D�p�̃V�F�[�_�[���g�p����
        spriteInitData.m_fxFilePath = "Assets/shader/postEffect/ssao.fx";
        spriteInitData.m_vsEntryPointFunc = "VSMain";
        spriteInitData.m_psEntryPoinFunc = "PSMain";

        spriteInitData.m_alphaBlendMode = AlphaBlendMode_None;
        spriteInitData.m_colorBufferFormat[0] = m_ssaoRenderTarget.GetColorBufferFormat();

        //�𑜓x��GPU�ɑ��邽�߂̒萔�o�b�t�@��ݒ肷��B
        spriteInitData.m_expandConstantBuffer = (void*)&m_cB;
        spriteInitData.m_expandConstantBufferSize = sizeof(SsaoBuffer) +
            (16 - (sizeof(SsaoBuffer) % 16));
        m_ssaoSprite.Init(spriteInitData);


        spriteInitData.m_textures[0] = &m_ssaoRenderTarget.GetRenderTargetTexture();
        // 2D�p�̃V�F�[�_�[���g�p����
        spriteInitData.m_fxFilePath = "Assets/shader/sprite.fx";
        spriteInitData.m_vsEntryPointFunc = "VSMain";
        spriteInitData.m_psEntryPoinFunc = "PSMain";
        spriteInitData.m_alphaBlendMode = AlphaBlendMode_Multiply;
        spriteInitData.m_colorBufferFormat[0] = mainRenderTarget.GetColorBufferFormat();

        m_finalSprite.Init(spriteInitData);
    }

    void Ssao::OnRender(RenderContext& rc, RenderTarget& mainRenderTarget)
    {
        //ssao�����_�\�^�[�Q�b�g���B
        //PRESENT����RENDERTARGET�ցB
        rc.WaitUntilToPossibleSetRenderTarget(m_ssaoRenderTarget);
        // �����_�����O�^�[�Q�b�g��ݒ�
        rc.SetRenderTargetAndViewport(m_ssaoRenderTarget);

        //�萔�o�b�t�@�[���X�V�B
        //�r���[�s��B
        m_cB.view = g_camera3D->GetViewMatrix();
        //�v���W�F�N�V�����s��B
        m_cB.proj = g_camera3D->GetProjectionMatrix();
        //�v���W�F�N�V�����s��̋t�s��B
        m_cB.invproj = g_camera3D->GetProjectionMatrixInv();

        //�`��B
        m_ssaoSprite.Draw(rc);

        //�����_�\�^�[�Q�b�g���B
        //���C�������_�\�^�[�Q�b�g�ɐݒ�B
        rc.SetRenderTarget(mainRenderTarget);

        //ssao�����_�\�^�[�Q�b�g���B
        //RENDERTARGET����PRESENT�ցB
        rc.WaitUntilFinishDrawingToRenderTarget(m_ssaoRenderTarget);

        //���C�������_�\�^�[�Q�b�g���B
        //PRESENT����RENDERTARGET�ցB
        rc.WaitUntilToPossibleSetRenderTarget(mainRenderTarget);

        //�`��B
        m_finalSprite.Draw(rc);

        rc.WaitUntilFinishDrawingToRenderTarget(mainRenderTarget);
    }
}
