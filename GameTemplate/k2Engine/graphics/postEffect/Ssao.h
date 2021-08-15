#pragma once

#include "graphics/postEffect/PostEffectComponentBase.h"

namespace nsK2Engine {
    /// <summary>
    /// Ssao(スクリーンスペースアンビエントオクルージョン)。
    /// </summary>
    class Ssao : public PostEffectComponentBase
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
        /// ポストの結果の画像をメインレンダリングターゲットにコピーする？
        /// </summary>
        /// <returns></returns>
        bool IsCopyResultTextureToMainRenderTarget() const override
        {
            return false;
        }
        /// <summary>
        /// ポストエフェクトを実行した結果となるテクスチャを取得。
        /// </summary>
        /// <returns></returns>
        Texture& GetResultTexture() override
        {
            return m_ssaoRenderTarget.GetRenderTargetTexture();
        }
    private:
        struct SsaoBuffer {
            Matrix view;//ビュー行列。
            Matrix proj;//プロジェクション行列。
            Matrix invproj;//逆プロジェクション行列。
        };
        RenderTarget m_ssaoRenderTarget;
        Sprite m_ssaoSprite;					//SSAO用のスプライト。
        Sprite m_finalSprite;					//最終合成用のスプライト。
        SsaoBuffer m_cB;                    //解像度をGPUに送るための定数バッファ―。
    };
}