#pragma once

#include "graphics/postEffect/PostEffectComponentBase.h"

namespace nsK2Engine {
    /// <summary>
    /// 被写界深度。
    /// </summary>
    class Dof : public PostEffectComponentBase
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
            return m_rtPhomboidBlur.GetRenderTargetTexture();
        }
        /// <summary>
        /// 被写界深度を有効にする。
        /// </summary>
        void Enable()
        {
            m_isEnable = true;
        }
        /// <summary>
        /// 被写界深度を無効にする。
        /// </summary>
        void Disable()
        {
            m_isEnable = false;
        }
    private:
        void InitCombimeBokeImageToSprite(Sprite& combineBokeImageSprite, Texture& bokeTexture, Texture& depthTexture);
    private:
        RenderTarget m_rtVerticalBlur;		//垂直ブラーをかけるためのレンダリングターゲット
        RenderTarget m_rtDiagonalBlur;		//対角線ブラーをかけるためのレンダリングターゲット
        RenderTarget m_rtPhomboidBlur;		//六角形ブラーをかけるためのレンダリングターゲット
        Sprite m_vertDIagonalBlurSprite;	//垂直、対角線ブラー用のスプライト
        Sprite m_phomboidBlurSprite;		//六角形ブラ用のスプライト
        Sprite m_combineBokeImageSprite;	//ボケ画像をメインレンダリングターゲットに合成するためのスプライト
        bool m_isEnable = false;            //被写界深度が有効？
    };
}
