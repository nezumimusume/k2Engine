#pragma once

#include "graphics/postEffect/PostEffectComponentBase.h"

namespace nsK2Engine {
    /// <summary>
    /// FXAA。
    /// </summary>
    class Fxaa : public PostEffectComponentBase
    {
    public:
        /// <summary>
        /// 初期化。
        /// </summary>
        /// <param name="mainRenderTarget">メインレンダーターゲット。</param>
        void OnInit(
            RenderTarget& mainRenderTarget,
            RenderTarget& zprepassRenderTarget,
            RenderTarget& normalRenderTarget,
            RenderTarget& metallicSmoothRenderTarget,
            RenderTarget& albedoRenderTarget) override;
        /// <summary>
        /// 描画。
        /// </summary>
        /// <param name="rc">レンダ―コンテキスト。</param>
        /// <param name="mainRenderTarget">メインレンダ―ターゲット。</param>
        void OnRender(RenderContext& rc, RenderTarget& mainRenderTarget) override;
        /// <summary>
        /// ポストエフェクトを実行した結果となるテクスチャを取得。
        /// </summary>
        /// <returns></returns>
        Texture& GetResultTexture() override
        {
            return m_fxaaRt.GetRenderTargetTexture();
        }
        /// <summary>
        /// ポストの結果の画像をメインレンダリングターゲットにコピーする？
        /// </summary>
        /// <returns></returns>
        bool IsCopyResultTextureToMainRenderTarget() const override
        {
            return true;
        }
    private:

        struct FaxxBuffer
        {
            float bufferW;
            float bufferH;
        };
        RenderTarget m_fxaaRt;  // FXAAを行うレンダリングターゲット。
        Sprite m_finalSprite;	// 最終合成用のスプライト
        Sprite m_copySprite;    //　
        FaxxBuffer m_cB;        // 解像度をGPUに送るための定数バッファ―。
    };
}