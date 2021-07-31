#pragma once

/// <summary>
/// 被写界深度。
/// </summary>
class Dof : public Noncopyable
{
public:
    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="mainRenderTarget">メインレンダリングターゲット</param>
    /// <param name="zprepassRenderTarget">ZPrepass</param>
    void Init(RenderTarget& mainRenderTarget, RenderTarget& zprepassRenderTarget);
    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="rc">レンダリングコンテキスト</param>
    /// <param name="mainRenderTarget">メインレンダリングターゲット</param>
    void Render(RenderContext& rc, RenderTarget& mainRenderTarget);
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

