#include "k2EnginePreCompile.h"
#include "Fxaa.h"

void Fxaa::Init(RenderTarget& mainRenderTarget)
{
    // 最終合成用のスプライトを初期化する
    SpriteInitData spriteInitData;
    // ボケテクスチャを4枚指定する
    spriteInitData.m_textures[0] = &mainRenderTarget.GetRenderTargetTexture();
    // 解像度はmainRenderTargetの幅と高さ
    spriteInitData.m_width = mainRenderTarget.GetWidth();
    spriteInitData.m_height = mainRenderTarget.GetHeight();
    // ぼかした画像を、通常の2Dとしてメインレンダリングターゲットに描画するので、
    // 2D用のシェーダーを使用する
    spriteInitData.m_fxFilePath = "Assets/shader/fxaa.fx";
    spriteInitData.m_vsEntryPointFunc = "VSMain";
    spriteInitData.m_psEntryPoinFunc = "PSMain";

    // ただし、加算合成で描画するので、アルファブレンディングモードを加算にする
    spriteInitData.m_alphaBlendMode = AlphaBlendMode_None;
    spriteInitData.m_colorBufferFormat[0] = mainRenderTarget.GetColorBufferFormat();

    m_finalSprite.Init(spriteInitData);

}

void Fxaa::Render(RenderContext& rc, RenderTarget& mainRenderTarget)
{
    // レンダリングターゲットとして利用できるまで待つ
    rc.WaitUntilToPossibleSetRenderTarget(mainRenderTarget);
    // レンダリングターゲットを設定
    rc.SetRenderTargetAndViewport(mainRenderTarget);
    //描画。
    m_finalSprite.Draw(rc);
    // レンダリングターゲットへの書き込み終了待ち
    rc.WaitUntilFinishDrawingToRenderTarget(mainRenderTarget);
}
