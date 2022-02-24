#include "k2EnginePreCompile.h"
#include "ToneMap.h"

namespace nsK2Engine {
	
	ToneMap::ToneMap()
	{

	}

	ToneMap::~ToneMap()
	{

	}

	void ToneMap::OnInit(
		RenderTarget& mainRenderTarget,
		RenderTarget& zprepassRenderTarget,
		RenderTarget& normalRenderTarget,
		RenderTarget& metallicSmoothRenderTarget,
		RenderTarget& albedoRenderTarget
	) {
		
		m_finalRt.Create(
			mainRenderTarget.GetWidth(),
			mainRenderTarget.GetHeight(),
			1,
			1,
			mainRenderTarget.GetColorBufferFormat(),
			DXGI_FORMAT_UNKNOWN
		);
		m_cb1.midddleGray = 0.18f;

		SpriteInitData initData;
		initData.m_width = mainRenderTarget.GetWidth();
		initData.m_height = mainRenderTarget.GetHeight();
		initData.m_colorBufferFormat[0] = mainRenderTarget.GetColorBufferFormat();
		initData.m_fxFilePath = "Assets/shader/postEffect/tonemap.fx";
		initData.m_psEntryPoinFunc = "PSFinal";
		initData.m_expandConstantBuffer = &m_cb1;
		initData.m_expandConstantBufferSize = sizeof(m_cb1);
		initData.m_textures[0] = &mainRenderTarget.GetRenderTargetTexture();
		initData.m_textures[1] = &g_renderingEngine->GetLuminanceAvgTextureInScene();
		
		m_finalSprite.Init(initData);
	}
	
	void ToneMap::OnRender(RenderContext& rc, RenderTarget& mainRenderTarget)
	{
		g_graphicsEngine->BeginGPUEvent("Tonemap");

		// 最終合成。
		// レンダリングターゲットとして利用できるまで待つ
		rc.WaitUntilToPossibleSetRenderTarget(m_finalRt);
		// レンダリングターゲットを設定
		rc.SetRenderTargetAndViewport(m_finalRt);
		// 最終合成。
		m_finalSprite.Draw(rc);
		// レンダリングターゲットへの書き込み終了待ち
		rc.WaitUntilFinishDrawingToRenderTarget(m_finalRt);

		g_graphicsEngine->EndGPUEvent();
	}
}