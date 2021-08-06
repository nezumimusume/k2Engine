#include "k2EnginePreCompile.h"
#include "PostEffect.h"

#include "graphics/effect/EffectEngine.h"

#include "RenderingEngine.h"

void PostEffect::Init(
    RenderTarget& mainRenderTarget, 
    RenderTarget& zprepassRenderTarget, 
    RenderTarget& normalRenderTarget,
    RenderTarget& metallicSmoothRenderTarget,
    RenderTarget& albedoRenderTarget
)
{
    m_bloom.Init(mainRenderTarget);
    m_dof.Init(mainRenderTarget, zprepassRenderTarget);
    m_fXaa.Init(mainRenderTarget);
    m_tonemap.Init(mainRenderTarget);
    m_ssao.Init(mainRenderTarget);
    m_ssr.Init(
        mainRenderTarget, 
        zprepassRenderTarget, 
        normalRenderTarget,
        metallicSmoothRenderTarget,
        albedoRenderTarget);
}
void PostEffect::Render(RenderContext& rc, RenderTarget& mainRenderTarget)
{   
    //m_ssao.Render(rc, mainRenderTarget);

    m_ssr.Render(rc, mainRenderTarget);
    m_tonemap.Render(rc, mainRenderTarget);

    g_renderingEngine->SetMainRenderTargetAndDepthStencilBuffer(rc);

    //メインレンダーターゲットをPRESENTからRENDERTARGETへ。
    rc.WaitUntilToPossibleSetRenderTarget(mainRenderTarget);
    //ここでエフェクトドローするよ。
    EffectEngine::GetInstance()->Draw();
    //メインレンダーターゲットをTARGETからPRESENTへ。
    rc.WaitUntilFinishDrawingToRenderTarget(mainRenderTarget);

    
    m_bloom.Render(rc, mainRenderTarget);
    m_dof.Render(rc, mainRenderTarget);
    
    m_fXaa.Render(rc, mainRenderTarget);
}