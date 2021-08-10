#pragma once

#include "graphics/PostEffectBase.h"

/// <summary>
/// スクリーンスペースリフレクション
/// </summary>
class Ssr : public PostEffectBase {
public:
	/// <summary>
	/// 初期化。
	/// </summary>
	void OnInit(
		RenderTarget& mainRenderTarget, 
		RenderTarget& depthRenderTarget,
		RenderTarget& normalRenderTarget,
		RenderTarget& metallicSmoothRenderTarget,
		RenderTarget& albedoRenderTarget
	) override;
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="rc"></param>
	void OnRender(RenderContext& rc, RenderTarget& mainRenderTarget) override;
	/// <summary>
	/// ポストの結果の画像をメインレンダリングターゲットにコピーする？
	/// </summary>
	/// <returns></returns>
	bool IsCopyResultTextureToMainRenderTarget() const override
	{
		return true;
	}
	/// <summary>
    /// ポストエフェクトを実行した結果となるテクスチャを取得。
    /// </summary>
    /// <returns></returns>
	Texture& GetResultTexture() override
	{
		return m_finalRt.GetRenderTargetTexture();
	}
	/// <summary>
	/// 有効にする。
	/// </summary>
	void Enable()
	{
		m_isEnable = true;
	}
	/// <summary>
	/// 無効にする。
	/// </summary>
	void Disable()
	{
		m_isEnable = false;
	}
private:
	struct SsrCb {
		Matrix mViewProjInv;				// ビュープロジェクション行列の逆行列。
		Matrix mViewProj;					// ビュープロジェクション行列。
		Vector3 cameraPosInWorld;			// ワールド座標系でのカメラの視点。
	};
	RenderTarget m_reflectionRt;			// 映り込みをを書きこむためのレンダリングターゲット。
	RenderTarget m_finalRt;					// 最終合成スプライト
	GaussianBlur m_blur;
	SsrCb m_ssrCb;
	Sprite m_sprite;
	Sprite m_finalSprite;					// 最終合成スプライト。
	bool m_isEnable = true;					// SSRの有効、無効。
};