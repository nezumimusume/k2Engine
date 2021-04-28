#pragma once
/// <summary>
/// FXAA。
/// </summary>
class Fxaa
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
    Sprite m_finalSprite;					//最終合成用のスプライト
};