#pragma once

#include "graphics/postEffect/PostEffectComponentBase.h"

namespace nsK2Engine {
    /// <summary>
    /// シーンの平均輝度の計算処理。
    /// </summary>
    class CalcSceneLuminance : public PostEffectComponentBase
	{
	public:
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
		/// ポストの結果の画像をメインレンダリングターゲットにコピーする？
		/// </summary>
		/// <returns></returns>
		bool IsCopyResultTextureToMainRenderTarget() const override
		{
			return false;
		}
		/// <summary>
		/// シーンが切り替わったことを通知。
		/// </summary>
		/// <param name="timer"></param>
		void NotifyChangeScene(float timer)
		{
			m_isFirstWhenChangeScene = timer;
			m_changeSceneTimer = timer;
		}
		/// <summary>
		/// シーンの平均輝度が記憶されているテクスチャを取得。
		/// </summary>
		/// <returns></returns>
		Texture& GetLuminanceAvgTextureInScene()
		{
			return m_luminanceAvgInTonemapRt.GetRenderTargetTexture();
		}
		
	private:
		/// <summary>
		/// 平均輝度を計算する。
		/// </summary>
		/// <param name="">レンダリングコンテキスト</param>
		void CalcLuminanceAvarage(RenderContext& rc);
	private:
		static const int MAX_SAMPLES = 16;

		// シェーダー側のb0レジスタに関連付けされるデータ。
		struct SCB_1 {
			float deltaTime;
		};
		enum CalcAvgSprite {
			enCalcAvgLog,					// 対数平均を求める。
			enCalcAvg_Start,
			enCalcAvg_0 = enCalcAvg_Start,	// 平均輝度を計算。
			enCalcAvg_1,					// 平均輝度を計算。
			enCalcAvg_2,					// 平均輝度を計算。	
			enCalcAvg_3,					// 平均輝度を計算する。
			enCalcAvg_End,
			enCalcAvgExp = enCalcAvg_End,	// exp()を用いて最終平均を求める。
			enNumCalcAvgSprite
		};
		RenderTarget m_calcAvgRt[enNumCalcAvgSprite];		// 平均輝度計算用のレンダリングターゲット。
		RenderTarget m_luminanceAvgInTonemapLastFrameRt;	// 1フレーム前のトーンマップで使用した平均輝度を記憶するためのレンダリングターゲット。
		RenderTarget m_luminanceAvgInTonemapRt;				// 現在のフレームのトーンマップで使用する平均輝度を記憶するためのレンダリングターゲット。			
		Sprite m_calcAvgSprites[enNumCalcAvgSprite];
		Sprite m_calcAdapteredLuminanceSprite;			// 明暗順応用のスプライト。
		Sprite m_calcAdapteredLuminanceFisrtSprite;		// 明暗順応用のスプライト。(シーンが切り替わったときに使用される。)
		Sprite m_copyLuminanceAvgInTonemapSprite;		// トーンマップで使用した平均輝度コピー用のスプライト。
		bool m_isFirstWhenChangeScene = true;			// シーンが切り替わって初回の描画かどうかのフラグ。
		Vector4 m_avSampleOffsets[MAX_SAMPLES];
		SCB_1 m_cb1;
		float m_changeSceneTimer = 0.5f;				// シーン切り替えタイマー。
	};
}