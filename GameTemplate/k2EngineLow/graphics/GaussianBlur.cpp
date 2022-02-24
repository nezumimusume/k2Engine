#include "k2EngineLowPreCompile.h"
#include "GaussianBlur.h"

namespace nsK2EngineLow {
	void GaussianBlur::Init(Texture* originalTexture, bool isBlurAlpha, int width, int height)
	{
		m_originalTexture = originalTexture;
		m_bokeTextureWidth = width;
		m_bokeTextureHeight = height;
		//レンダリングターゲットを初期化。
		InitRenderTargets();
		//スプライトを初期化。
		InitSprites(isBlurAlpha);
	}
	void GaussianBlur::Init(Texture* originalTexture, bool isBlurAlpha, bool isDownSample)
	{
		if (isDownSample) {
			m_bokeTextureWidth = originalTexture->GetWidth() / 2.0f;
			m_bokeTextureHeight = originalTexture->GetHeight() / 2.0f;
		}
		else {
			m_bokeTextureWidth = originalTexture->GetWidth();
			m_bokeTextureHeight = originalTexture->GetHeight();
		}
		m_originalTexture = originalTexture;

		//レンダリングターゲットを初期化。
		InitRenderTargets();
		//スプライトを初期化。
		InitSprites(isBlurAlpha);
	}


	void GaussianBlur::ExecuteOnGPU(RenderContext& rc, float blurPower)
	{
		//重みテーブルを更新する。
		UpdateWeightsTable(blurPower);

		//横ブラーを実行。
		//レンダリングターゲットとして利用できるようになるまでwaitを入れる。
		rc.WaitUntilToPossibleSetRenderTarget(m_xBlurRenderTarget);
		//レンダリングターゲットを設定。
		rc.SetRenderTargetAndViewport(m_xBlurRenderTarget);

		//ドロー。
		m_xBlurSprite.Draw(rc);
		//レンダリングターゲットへの書き込み終了待ち。
		rc.WaitUntilFinishDrawingToRenderTarget(m_xBlurRenderTarget);

		//縦ブラーを実行。
		//レンダリングターゲットとして利用できるようになるまでwaitを入れる。
		rc.WaitUntilToPossibleSetRenderTarget(m_yBlurRenderTarget);
		//レンダリングターゲットを設定。
		rc.SetRenderTargetAndViewport(m_yBlurRenderTarget);

		//ドロー。
		m_yBlurSprite.Draw(rc);
		//レンダリングターゲットへの書き込み終了待ち。
		rc.WaitUntilFinishDrawingToRenderTarget(m_yBlurRenderTarget);
	}

	void GaussianBlur::InitRenderTargets()
	{
		int w = m_originalTexture->GetWidth();
		int h = m_originalTexture->GetHeight();
		//Xブラー用のレンダリングターゲットを作成する。
		m_xBlurRenderTarget.Create(
			m_bokeTextureWidth,
			h,
			1,
			1,
			m_originalTexture->GetFormat(),
			DXGI_FORMAT_UNKNOWN
		);

		//Yブラー用のレンダリングターゲットを作成する。
		m_yBlurRenderTarget.Create(
			m_bokeTextureWidth,
			m_bokeTextureHeight,
			1,
			1,
			m_originalTexture->GetFormat(),
			DXGI_FORMAT_UNKNOWN
		);
	}

	void GaussianBlur::InitSprites(bool isBlurAlpha)
	{
		const char* psEntryFunc = nullptr;
		if (isBlurAlpha) {
			psEntryFunc = "PSBlur_Alpha";
		}
		else {
			psEntryFunc = "PSBlur";
		}
		//横ブラー用のスプライトを初期化する。
		{
			SpriteInitData xBlurSpriteInitData;
			xBlurSpriteInitData.m_fxFilePath = "Assets/shader/util/gaussianBlur.fx";
			xBlurSpriteInitData.m_vsEntryPointFunc = "VSXBlur";
			xBlurSpriteInitData.m_psEntryPoinFunc = psEntryFunc;
			//スプライトの解像度はm_xBlurRenderTargetと同じ。
			xBlurSpriteInitData.m_width = m_xBlurRenderTarget.GetWidth();
			xBlurSpriteInitData.m_height = m_xBlurRenderTarget.GetHeight();
			//テクスチャは輝度テクスチャ。
			xBlurSpriteInitData.m_textures[0] = m_originalTexture;
			//描き込むレンダリングターゲットのフォーマットを指定する。
			xBlurSpriteInitData.m_colorBufferFormat[0] = m_xBlurRenderTarget.GetColorBufferFormat();
			//ユーザー拡張の定数バッファにブラー用のパラメーターを設定する。
			xBlurSpriteInitData.m_expandConstantBuffer = &m_weights;
			xBlurSpriteInitData.m_expandConstantBufferSize = sizeof(m_weights);

			//初期化情報をもとに横ブラー用のスプライトを初期化する。
			m_xBlurSprite.Init(xBlurSpriteInitData);
		}
		//縦ブラー用のスプライトを初期化する。
		{
			SpriteInitData yBlurSpriteInitData;
			yBlurSpriteInitData.m_fxFilePath = "Assets/shader/util/gaussianBlur.fx";
			yBlurSpriteInitData.m_vsEntryPointFunc = "VSYBlur";
			yBlurSpriteInitData.m_psEntryPoinFunc = psEntryFunc;
			//スプライトの解像度はm_yBlurRenderTargetと同じ。
			yBlurSpriteInitData.m_width = m_yBlurRenderTarget.GetWidth();
			yBlurSpriteInitData.m_height = m_yBlurRenderTarget.GetHeight();
			//テクスチャは横ブラーをかけたもの。
			yBlurSpriteInitData.m_textures[0] = &m_xBlurRenderTarget.GetRenderTargetTexture();
			//描き込むレンダリングターゲットのフォーマットを指定する。
			yBlurSpriteInitData.m_colorBufferFormat[0] = m_yBlurRenderTarget.GetColorBufferFormat();
			//ユーザー拡張の定数バッファにブラー用のパラメーターを設定する。
			yBlurSpriteInitData.m_expandConstantBuffer = &m_weights;
			yBlurSpriteInitData.m_expandConstantBufferSize = sizeof(m_weights);

			//初期化情報をもとに縦ブラー用のスプライトを初期化する。
			m_yBlurSprite.Init(yBlurSpriteInitData);
		}
	}
	void GaussianBlur::UpdateWeightsTable(float blurPower)
	{
		float total = 0;
		for (int i = 0; i < NUM_WEIGHTS; i++) {
			m_weights[i] = expf(-0.5f * (float)(i * i) / blurPower);
			total += 2.0f * m_weights[i];

		}
		// 規格化
		for (int i = 0; i < NUM_WEIGHTS; i++) {
			m_weights[i] /= total;
		}
	}
}