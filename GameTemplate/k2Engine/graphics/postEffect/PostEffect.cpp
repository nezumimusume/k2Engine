#include "k2EnginePreCompile.h"
#include "PostEffect.h"
#include "graphics/effect/EffectEngine.h"
#include "Graphics/RenderingEngine.h"

namespace nsK2Engine {
    void PostEffect::Init(
        RenderTarget& mainRenderTarget,
        RenderTarget& zprepassRenderTarget,
        RenderTarget& normalRenderTarget,
        RenderTarget& metallicSmoothRenderTarget,
        RenderTarget& albedoRenderTarget
    )
    {
        m_bloom.Init(
            mainRenderTarget,
            zprepassRenderTarget,
            normalRenderTarget,
            metallicSmoothRenderTarget,
            albedoRenderTarget
        );
        m_dof.Init(
            mainRenderTarget,
            zprepassRenderTarget,
            normalRenderTarget,
            metallicSmoothRenderTarget,
            albedoRenderTarget
        );
        m_fXaa.Init(
            mainRenderTarget,
            zprepassRenderTarget,
            normalRenderTarget,
            metallicSmoothRenderTarget,
            albedoRenderTarget
        );
        m_tonemap.Init(
            mainRenderTarget,
            zprepassRenderTarget,
            normalRenderTarget,
            metallicSmoothRenderTarget,
            albedoRenderTarget
        );
        m_ssao.Init(
            mainRenderTarget,
            zprepassRenderTarget,
            normalRenderTarget,
            metallicSmoothRenderTarget,
            albedoRenderTarget
        );
        m_ssr.Init(
            mainRenderTarget,
            zprepassRenderTarget,
            normalRenderTarget,
            metallicSmoothRenderTarget,
            albedoRenderTarget
        );
    }
    void PostEffect::Render(RenderContext& rc, RenderTarget& mainRenderTarget)
    {
        //m_ssao.Render(rc, mainRenderTarget);
        // SSR
        m_ssr.Render(rc, mainRenderTarget);
        // �g�[���}�b�v
        m_tonemap.Render(rc, mainRenderTarget);

        g_renderingEngine->SetMainRenderTargetAndDepthStencilBuffer(rc);

        //���C�������_�[�^�[�Q�b�g��PRESENT����RENDERTARGET�ցB
        rc.WaitUntilToPossibleSetRenderTarget(mainRenderTarget);
        //�����ŃG�t�F�N�g�h���[�B
        EffectEngine::GetInstance()->Draw();
        //���C�������_�[�^�[�Q�b�g��TARGET����PRESENT�ցB
        rc.WaitUntilFinishDrawingToRenderTarget(mainRenderTarget);
        // �u���[��
        m_bloom.Render(rc, mainRenderTarget);
        // DOF
        m_dof.Render(rc, mainRenderTarget);
        // FXAA
        m_fXaa.Render(rc, mainRenderTarget);
    }
}