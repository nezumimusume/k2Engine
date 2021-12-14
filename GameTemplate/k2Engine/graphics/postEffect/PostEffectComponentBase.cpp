#include "k2EnginePreCompile.h"
#include "PostEffectComponentBase.h"

namespace nsK2Engine {
	void PostEffectComponentBase::Init(
		RenderTarget& mainRenderTarget,
		RenderTarget& zprepassRenderTarget,
		RenderTarget& normalRenderTarget,
		RenderTarget& metallicSmoothRenderTarget,
		RenderTarget& albedoRenderTarget
	)
	{
		OnInit(
			mainRenderTarget,
			zprepassRenderTarget,
			normalRenderTarget,
			metallicSmoothRenderTarget,
			albedoRenderTarget
		);

		if (IsCopyResultTextureToMainRenderTarget()) {
			SpriteInitData initData;
			initData.m_width = mainRenderTarget.GetWidth();
			initData.m_height = mainRenderTarget.GetHeight();
			initData.m_colorBufferFormat[0] = mainRenderTarget.GetColorBufferFormat();
			initData.m_fxFilePath = "Assets/shader/Sprite.fx";
			initData.m_textures[0] = &GetResultTexture();
			m_copyMainRtSprite.Init(initData);
		}
	}

	void PostEffectComponentBase::Render(RenderContext& rc, RenderTarget& mainRenderTarget)
	{
		if (!m_isEnable) {
			return;
		}
		OnRender(rc, mainRenderTarget);
		if (IsCopyResultTextureToMainRenderTarget()) {
			// レンダリングターゲットとして利用できるまで待つ
			rc.WaitUntilToPossibleSetRenderTarget(mainRenderTarget);
			// レンダリングターゲットを設定
			rc.SetRenderTargetAndViewport(mainRenderTarget);
			// ポストエフェクトの結果をメインレンダリングターゲットに反映。
			m_copyMainRtSprite.Draw(rc);
			// レンダリングターゲットへの書き込み終了待ち
			rc.WaitUntilFinishDrawingToRenderTarget(mainRenderTarget);
		}
	}
}