#pragma once


/// <summary>
/// スクリーンスペースリフレクション
/// </summary>
class Ssr {
public:
	/// <summary>
	/// 初期化。
	/// </summary>
	void Init(
		RenderTarget& mainRenderTarget, 
		RenderTarget& depthRenderTarget,
		RenderTarget& normalRenderTarget,
		RenderTarget& metallicSmoothRenderTarget,
		RenderTarget& albedoRenderTarget
	);
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="rc"></param>
	void Render(RenderContext& rc, RenderTarget& mainRenderTarget);
private:
	struct SsrCb {
		Matrix mViewProjInv;				//ビュープロジェクション行列の逆行列。
		Matrix mViewProj;					//ビュープロジェクション行列。
		Vector3 cameraPosInWorld;			//ワールド座標系でのカメラの視点。
	};
	RenderTarget m_reflectionRt;				//映り込みをを書きこむためのレンダリングターゲット。
	GaussianBlur m_blur;
	SsrCb m_ssrCb;
	Sprite m_sprite;
	Sprite m_finalSprite;						//最終合成スプライト。
};