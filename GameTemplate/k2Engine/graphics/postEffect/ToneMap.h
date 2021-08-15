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
		/// シーンが切り替わったことを通知。
		/// </summary>
		/// <param name="timer"></param>
		void NotifyChangeScene(float timer)
		{
			m_isFirstWhenChangeScene = timer;
			m_changeSceneTimer = timer;
		}
		/// <summary>
		/// トーンマップを有効にする。
		/// </summary>
		void Enable()
		{
			m_isEnable = true;
		}
		/// <summary>
		/// トーンマップを無効にする。
		/// </summary>
		void Disable()
		{
			m_isEnable = false;
		}
	private:
		/// <summary>
		/// 平均輝度を計算する。
		/// </summary>
		/// <param name="">レンダリングコンテキスト</param>
		void CalcLuminanceAvarage(RenderContext& rc);
	private:
		static const int MAX_SAMPLES = 16;
		struct STonemapParam {
			float deltaTime;
			float midddleGray;
			int currentAvgTexNo;
		};
		enum CalcAvgSprite {
			enCalcAvgLog,						// 対数平均を求める。
			enCalcAvg_Start,
			enCalcAvg_0 = enCalcAvg_Start,	// 平均輝度を計算。
			enCalcAvg_1,					// 平均輝度を計算。
			enCalcAvg_2,					// 平均輝度を計算。	
			enCalcAvg_3,					// 平均輝度を計算する。
			enCalcAvg_End,
			enCalcAvgExp = enCalcAvg_End,		// exp()を用いて最終平均を求める。
			enNumCalcAvgSprite
		};
		RenderTarget m_calcAvgRt[enNumCalcAvgSprite];	// 平均輝度計算用のレンダリングターゲット。
		RenderTarget m_avgRt[2];					// 平均輝度が格納されるレンダリングターゲット。
		int m_currentAvgRt = 0;						// 
		Sprite m_calcAvgSprites[enNumCalcAvgSprite];
		Sprite m_calcAdapteredLuminanceSprite;		// 明暗順応用のスプライト。
		Sprite m_calcAdapteredLuminanceFisrtSprite;	// 明暗順応用のスプライト。(シーンが切り替わったときに使用される。)
		Sprite m_finalSprite;						// 最終合成用のスプライト。

		bool m_isFirstWhenChangeScene = true;			//!<シーンが切り替わって初回の描画かどうかのフラグ。
		Vector4 m_avSampleOffsets[MAX_SAMPLES];
		RenderTarget m_finalRt;						// 最終合成レンダリングターゲット。
		STonemapParam m_tonemapParam;
		bool m_isEnable = true;					// トーンマップが有効？
		float m_changeSceneTimer = 0.5f;		// シーン切り替えタイマー。
	};
}