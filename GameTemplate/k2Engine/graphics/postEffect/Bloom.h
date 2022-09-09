#pragma once

#include "graphics/postEffect/PostEffectComponentBase.h"

namespace nsK2Engine {
    /// <summary>
    /// ブルーム。
    /// </summary>
    class Bloom : public PostEffectComponentBase
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
            return m_luminanceRenderTarget.GetRenderTargetTexture();
        }
        /// <summary>
        /// ブルームが発生する閾値を設定。
        /// </summary>
        /// <param name="value"></param>
        void SetThreshold(float value)
        {
            m_samplingLuminanceCB1.threshold = value;
        }
        /// <summary>
        /// ブルームが発生する閾値を取得。
        /// </summary>
        /// <returns></returns>
        float GetThreshold() const
        {
            return m_samplingLuminanceCB1.threshold;
        }
    private:
        /// <summary>
        /// 輝度抽出パスの定数バッファ１に送る内容。
        /// </summary>
        struct SSamplingLuminanceCB1 {
            int isTonemap;      // トーンマップが有効？
            float middlegray;   // ミドルグレー。
            float threshold = 1.0f;
        };
        RenderTarget m_luminanceRenderTarget;	//輝度抽出用のレンダリングターゲット
        Sprite m_luminanceSprite;				//輝度抽出用のスプライト
        GaussianBlur m_gaussianBlur[4];			//ガウシアンブラー
        Sprite m_finalSprite;					//最終合成用のスプライト
        SSamplingLuminanceCB1 m_samplingLuminanceCB1;
    };
}