#pragma once

#include "graphics/postEffect/PostEffectComponentBase.h"

namespace nsK2Engine {
    class ToneMap : public PostEffectComponentBase
	{
	public:
		ToneMap();
		~ToneMap();
		/// <summary>
		/// 描画。
		/// </summary>
		/// <param name="rc">レンダ―コンテキスト。</param>
		/// <param name="mainRenderTarget">メインレンダ―ターゲット。</param>
		void OnRender(RenderContext& rc, RenderTarget& mainRenderTarget) override;
		/// <summary>
		/// 初期化。
		/// </summary>
		void OnInit(
			RenderTarget& mainRenderTarget,
			RenderTarget& zprepassRenderTarget,
			RenderTarget& normalRenderTarget,
			RenderTarget& metallicSmoothRenderTarget,
			RenderTarget& albedoRenderTarget) override;
		/// <summary>
		/// ポストエフェクトを実行した結果となるテクスチャを取得。
		/// </summary>
		/// <returns></returns>
		Texture& GetResultTexture() override
		{
			return m_finalRt.GetRenderTargetTexture();
		}
		/// <summary>
		/// ポストの結果の画像をメインレンダリングターゲットにコピーする？
		/// </summary>
		/// <returns></returns>
		bool IsCopyResultTextureToMainRenderTarget() const override
		{
			return true;
		}
		
		/// <summary>
		/// シーンの明るさの基準値を設定する。
		/// </summary>
		/// <param name="middlegray"></param>
		void SetMiddlegray(float middlegray)
		{
			m_cb1.midddleGray = middlegray;
		}
		/// <summary>
		/// シーンの明るさの基準値を取得する。
		/// </summary>
		/// <param name="middlegray"></param>
		float GetMiddlegray() const
		{
			return m_cb1.midddleGray;
		}
		
	private:
		static const int MAX_SAMPLES = 16;
		struct SCB_1 {
			float midddleGray;
		};
		Sprite m_finalSprite;							// 最終合成用のスプライト。
		RenderTarget m_finalRt;							// 最終合成レンダリングターゲット。
		SCB_1 m_cb1;
	};
}