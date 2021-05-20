#pragma once

/// <summary>
/// ブルーム。
/// </summary>
class Bloom
{
public:
    /// <summary>
    /// 初期化。
    /// </summary>
    /// <param name="mainRenderTarget">メインレンダーターゲット。</param>
    void Init(RenderTarget& mainRenderTarget);
    /// <summary>
    /// 描画。
    /// </summary>
    /// <param name="rc">レンダ―コンテキスト。</param>
    /// <param name="mainRenderTarget">メインレンダ―ターゲット。</param>
    void Render(RenderContext& rc, RenderTarget& mainRenderTarget);
private:
    RenderTarget m_luminanceRenderTarget;	//輝度抽出用のレンダリングターゲット
    Sprite m_luminanceSprite;				//輝度抽出用のスプライト
    GaussianBlur m_gaussianBlur[4];			//ガウシアンブラー
    Sprite m_finalSprite;					//最終合成用のスプライト
};
