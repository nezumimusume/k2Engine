#pragma once

#include "graphics/postEffect/PostEffectComponentBase.h"

namespace nsK2Engine {
	/// <summary>
	/// スクリーンスペースリフレクション
	/// </summary>
	/// <remark>
	/// レイマーチング法によるSSR。
	/// 反射が発生するのは、G-Bufferに書き込まれたSmoothパラメーターに依存する。
	/// Smoothが0.0のピクセルでは反射は発生しない。最も強い反射はsmoothの値が1.0のときとなる。
	/// Smoothの扱いは、本エンジンのPBRのワークフローを参照。
	/// </reamrk>
	class Ssr : public PostEffectComponentBase {
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
		/// <param name="rc">レンダリングコンテキスト</param>
		/// <param name="mainRenderTarget">メインレンダリングターゲット</param>
		void OnRender(RenderContext& rc, RenderTarget& mainRenderTarget) override;
		/// <summary>
		/// ポストの結果の画像をメインレンダリングターゲットにコピーする？
		/// </summary>
		/// <returns>
		/// trueを返すと、GetResultTexture()関数が返してくる、
		/// ポストエフェクトの結果となるテクスチャをメインレンダリングターゲットにコピーします。
		/// OnRender()関数の中で、すでに結果をメインレンダリングターゲットに書き込み済みであるなら、
		/// falseを返してください。
		/// </returns>
		bool IsCopyResultTextureToMainRenderTarget() const override
		{
			return true;
		}
		/// <summary>
		/// ポストエフェクトを実行した結果となるテクスチャを取得。
		/// </summary>
		/// <returns>
		/// ポストエフェクトの実行結果となるテクスチャ。
		/// IsCopyResultTextureToMainRenderTarget()関数がtrueを返す場合、
		/// OnRender()関数の実行後にメインレンダリングターゲットに、このテクスチャの内容が
		/// 貼り付けられます。
		/// IsCopyResultTextureToMainRenderTarget()関数がfalseを返す場合は、
		/// 本関数は利用されません。
		/// </returns>
		Texture& GetResultTexture() override
		{
			return m_finalRt.GetRenderTargetTexture();
		}
		/// <summary>
		/// SSRを有効にする。
		/// </summary>
		void Enable()
		{
			m_isEnable = true;
		}
		/// <summary>
		/// SSRを無効にする。
		/// </summary>
		void Disable()
		{
			m_isEnable = false;
		}
	private:
		/// <summary>
		/// GPUに転送するデータ構造体。
		/// </summary>
		struct SsrCb {
			Matrix mViewProjInv;				// ビュープロジェクション行列の逆行列。
			Matrix mViewProj;					// ビュープロジェクション行列。
			Vector3 cameraPosInWorld;			// ワールド座標系でのカメラの視点。
		};
		RenderTarget m_reflectionRt;			// 映り込みをを書きこむためのレンダリングターゲット。
		RenderTarget m_finalRt;					// 最終合成スプライト
		GaussianBlur m_blur;					// ブラー
		SsrCb m_ssrCb;							// GPUに転送するデータ。
		Sprite m_reflectionSprite;				// 映り込み画像を作成するためのスプライト
		Sprite m_finalSprite;					// 最終合成スプライト。
		bool m_isEnable = true;					// SSRの有効、無効。
	};
}