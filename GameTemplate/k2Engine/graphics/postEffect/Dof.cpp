#include "k2EnginePreCompile.h"
#include "Dof.h"

namespace nsK2Engine {
    void Dof::InitCombimeBokeImageToSprite(Sprite& combineBokeImageSprite, Texture& bokeTexture, Texture& depthTexture)
    {
        SpriteInitData combineBokeImageSpriteInitData;
        //�g�p����e�N�X�`���͂Q��
        combineBokeImageSpriteInitData.m_textures[0] = &bokeTexture;
        combineBokeImageSpriteInitData.m_textures[1] = &depthTexture;
        combineBokeImageSpriteInitData.m_width = 1280;
        combineBokeImageSpriteInitData.m_height = 720;
        combineBokeImageSpriteInitData.m_fxFilePath = "Assets/shader/postEffect/dof.fx";
        combineBokeImageSpriteInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
        // �����𗘗p���ă{�P�摜���A���t�@�u�����f�B���O����̂ŁA�������������[�h�ɂ���
        combineBokeImageSpriteInitData.m_alphaBlendMode = AlphaBlendMode_Trans;
        // �������I�u�W�F�N�g�𗘗p���ăX�v���C�g������������

        combineBokeImageSprite.Init(combineBokeImageSpriteInitData);
    }
    void Dof::OnInit(
        RenderTarget& mainRenderTarget,
        RenderTarget& zprepassRenderTarget,
        RenderTarget& normalRenderTarget,
        RenderTarget& metallicSmoothRenderTarget,
        RenderTarget& albedoRenderTarget
    )
    {
        m_rtVerticalBlur.Create(
            mainRenderTarget.GetWidth(),
            mainRenderTarget.GetHeight(),
            1,
            1,
            mainRenderTarget.GetColorBufferFormat(),
            DXGI_FORMAT_UNKNOWN
        );
        m_rtDiagonalBlur.Create(
            mainRenderTarget.GetWidth(),
            mainRenderTarget.GetHeight(),
            1,
            1,
            mainRenderTarget.GetColorBufferFormat(),
            DXGI_FORMAT_UNKNOWN
        );
        m_rtPhomboidBlur.Create(
            mainRenderTarget.GetWidth(),
            mainRenderTarget.GetHeight(),
            1,
            1,
            mainRenderTarget.GetColorBufferFormat(),
            DXGI_FORMAT_UNKNOWN
        );

        // step-2 �����A�Ίp���u���[�������邽�߂̃X�v���C�g��������
        SpriteInitData vertDiagonalBlurSpriteInitData;
        vertDiagonalBlurSpriteInitData.m_textures[0] = &mainRenderTarget.GetRenderTargetTexture();
        vertDiagonalBlurSpriteInitData.m_width = mainRenderTarget.GetWidth();
        vertDiagonalBlurSpriteInitData.m_height = mainRenderTarget.GetHeight();
        vertDiagonalBlurSpriteInitData.m_fxFilePath = "Assets/shader/util/hexaBlur.fx";

        // �����A�Ίp���u���[�p�̃s�N�Z���V�F�[�_�[���w�肷��
        vertDiagonalBlurSpriteInitData.m_psEntryPoinFunc = "PSVerticalDiagonalBlur";
        vertDiagonalBlurSpriteInitData.m_colorBufferFormat[0] = mainRenderTarget.GetColorBufferFormat();
        vertDiagonalBlurSpriteInitData.m_colorBufferFormat[1] = mainRenderTarget.GetColorBufferFormat();

        m_vertDIagonalBlurSprite.Init(vertDiagonalBlurSpriteInitData);

        // step-3 �Z�p�`�u���[�������邽�߂̃X�v���C�g��������
        SpriteInitData phomboidBlurSpriteInitData;
        phomboidBlurSpriteInitData.m_textures[0] = &m_rtVerticalBlur.GetRenderTargetTexture();
        phomboidBlurSpriteInitData.m_textures[1] = &m_rtDiagonalBlur.GetRenderTargetTexture();
        phomboidBlurSpriteInitData.m_width = mainRenderTarget.GetWidth();
        phomboidBlurSpriteInitData.m_height = mainRenderTarget.GetHeight();
        phomboidBlurSpriteInitData.m_fxFilePath = "Assets/shader/util/hexaBlur.fx";

        // �Z�p�`�u���[�p�̃s�N�Z���V�F�[�_�[���w�肷��
        phomboidBlurSpriteInitData.m_psEntryPoinFunc = "PSRhomboidBlur";
        phomboidBlurSpriteInitData.m_colorBufferFormat[0] = mainRenderTarget.GetColorBufferFormat();

        m_phomboidBlurSprite.Init(phomboidBlurSpriteInitData);

        InitCombimeBokeImageToSprite(
            m_combineBokeImageSprite,                     // �����������X�v���C�g
            m_rtPhomboidBlur.GetRenderTargetTexture(),    // �{�P�e�N�X�`��
            zprepassRenderTarget.GetRenderTargetTexture()  // �[�x�e�N�X�`��
        );

    }
    void Dof::OnRender(RenderContext& rc, RenderTarget& mainRenderTarget)
    {
        if (m_isEnable == false) {
            return;
        }
        g_graphicsEngine->BeginGPUEvent("Dof");
        //step-4 �����A�Ίp���u���[��������
        RenderTarget* blurRts[] = {
            &m_rtVerticalBlur,
            &m_rtDiagonalBlur
        };

        //�����_�����O�^�[�Q�b�g�Ƃ��ė��p�ł���܂ő҂�
        rc.WaitUntilToPossibleSetRenderTargets(2, blurRts);
        //�����_�����O�^�[�Q�b�g��ݒ�
        rc.SetRenderTargetsAndViewport(2, blurRts);
        // �����_�����O�^�[�Q�b�g���N���A
        rc.ClearRenderTargetViews(2, blurRts);
        //
        m_vertDIagonalBlurSprite.Draw(rc);
        // �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
        rc.WaitUntilFinishDrawingToRenderTargets(2, blurRts);

        //step-5 �Z�p�`�u���[��������
        rc.WaitUntilToPossibleSetRenderTarget(m_rtPhomboidBlur);
        rc.SetRenderTargetAndViewport(m_rtPhomboidBlur);

        m_phomboidBlurSprite.Draw(rc);

        // �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
        rc.WaitUntilFinishDrawingToRenderTarget(m_rtPhomboidBlur);

        // �{�P�摜�Ɛ[�x�e�N�X�`���𗘗p���āA�{�P�摜��`������ł���
        // ���C�������_�����O�^�[�Q�b�g��ݒ�
        rc.WaitUntilToPossibleSetRenderTarget(mainRenderTarget);
        rc.SetRenderTargetAndViewport(mainRenderTarget);

        // �X�v���C�g��`��&
        m_combineBokeImageSprite.Draw(rc);

        // �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
        rc.WaitUntilFinishDrawingToRenderTarget(mainRenderTarget);
        g_graphicsEngine->EndGPUEvent();
    };
}