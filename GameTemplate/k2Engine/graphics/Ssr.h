#pragma once

#include <random>

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
		float pad;
		float rayMarchStepRate;				//レイマーチのステップレート。。
	};
	RenderTarget m_reflectionRt;				//映り込みをを書きこむためのレンダリングターゲット。
	GaussianBlur m_blur;
	SsrCb m_ssrCb;
	Sprite m_sprite;
	Sprite m_finalSprite;						//最終合成スプライト。
	std::random_device m_rd;					//!<他に影響を与えないように、こいつが内部で乱数を保持する。
	std::mt19937 m_mt;							//!<メルセンヌツイスターを使用した乱数生成器。
};