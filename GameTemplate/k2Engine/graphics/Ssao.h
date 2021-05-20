#pragma once
/// <summary>
/// Ssao(スクリーンスペースアンビエントオクルージョン)。
/// </summary>
class Ssao
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
    struct SsaoBuffer{
        Matrix view;//ビュー行列。
        Matrix proj;//プロジェクション行列。
        Matrix invproj;//逆プロジェクション行列。
    };
    RenderTarget m_ssaoRenderTarget;
    Sprite m_ssaoSprite;					//SSAO用のスプライト。
    Sprite m_finalSprite;					//最終合成用のスプライト。
    SsaoBuffer m_cB;                    //解像度をGPUに送るための定数バッファ―。
};
