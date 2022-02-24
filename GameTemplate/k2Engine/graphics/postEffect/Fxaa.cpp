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
        // 最終合成用のスプライトを初期化する
        SpriteInitData spriteInitData;
        spriteInitData.m_textures[0] = &mainRenderTarget.GetRenderTargetTexture();
        // 解像度はmainRenderTargetの幅と高さ
        spriteInitData.m_width = mainRenderTarget.GetWidth();
        spriteInitData.m_height = mainRenderTarget.GetHeight();
        // 2D用のシェーダーを使用する
        spriteInitData.m_fxFilePath = "Assets/shader/postEffect/fxaa.fx";
        spriteInitData.m_vsEntryPointFunc = "VSMain";
        spriteInitData.m_psEntryPoinFunc = "PSMain";

        spriteInitData.m_alphaBlendMode = AlphaBlendMode_None;

        //解像度をGPUに送るための定数バッファを設定する。
        spriteInitData.m_expandConstantBuffer = (void*)&m_cB;
        spriteInitData.m_expandConstantBufferSize = sizeof(FaxxBuffer) +
            (16 - (sizeof(FaxxBuffer) % 16));
        m_finalSprite.Init(spriteInitData);

        m_fxaaRt.Create(
            mainRenderTarget.GetWidth(),
            mainRenderTarget.GetHeight(),
            1,
            1,
            DXGI_FORMAT_R8G8B8A8_UNORM, // HDR系のエフェクトは終了しているので、8bit整数バッファでＯＫ。
            DXGI_FORMAT_UNKNOWN
        );


    }

    void Fxaa::OnRender(RenderContext& rc, RenderTarget& mainRenderTarget)
    {
        g_graphicsEngine->BeginGPUEvent("FXAA");
        // レンダリングターゲットとして利用できるまで待つ
        rc.WaitUntilToPossibleSetRenderTarget(m_fxaaRt);
        // レンダリングターゲットを設定
        rc.SetRenderTargetAndViewport(m_fxaaRt);
        m_cB.bufferW = static_cast<float>(g_graphicsEngine->GetFrameBufferWidth());
        m_cB.bufferH = static_cast<float>(g_graphicsEngine->GetFrameBufferHeight());
        //描画。
        m_finalSprite.Draw(rc);
        // レンダリングターゲットへの書き込み終了待ち
        //メインレンダ―ターゲットをRENDERTARGETからPRESENTへ。
        rc.WaitUntilFinishDrawingToRenderTarget(m_fxaaRt);
        g_graphicsEngine->EndGPUEvent();
    }
}