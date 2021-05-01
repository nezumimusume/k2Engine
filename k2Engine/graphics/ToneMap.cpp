#include "k2EnginePreCompile.h"
#include "ToneMap.h"

ToneMap::ToneMap()
{

}

ToneMap::~ToneMap()
{

}

void ToneMap::Init(RenderTarget& mainRenderTarget)
{
	int width = g_graphicsEngine->GetFrameBufferWidth();
	int height = g_graphicsEngine->GetFrameBufferHeight();

	int number = 0;
	while (true)
	{
		width /= 2;
		height /= 2;
	
		if (width == 0)
		{
			width = 1;
		}
		if (height == 0)
		{
			height = 1;
		}

		float color[4] =
		{
			0.0f,0.0f,0.0f,0.0f
		};

		//サイズを1/4にしたレンダ―ターゲットを作成。
		auto renderPtr = std::make_unique<RenderTarget>();
		renderPtr.get()->Create(width, height,
			1,
			1,
			DXGI_FORMAT_R32_FLOAT,
			DXGI_FORMAT_UNKNOWN,
			color);
		m_renderTargetVector.push_back(std::move(renderPtr));

		//平均輝度を計算するためのスプライトを初期化する。

		SpriteInitData spriteInitData;
		if (number == 0)
		{
			spriteInitData.m_textures[0] = &mainRenderTarget.GetRenderTargetTexture();
			spriteInitData.m_colorBufferFormat[0] = mainRenderTarget.GetColorBufferFormat();
			spriteInitData.m_psEntryPoinFunc = "ExtractLumaPSMain";
		}
		else
		{
			spriteInitData.m_textures[0] = &m_renderTargetVector[number - 1].get()->GetRenderTargetTexture();
			spriteInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32_FLOAT;
			spriteInitData.m_psEntryPoinFunc = "PSMain";
		}

		//解像度は4/1したもの。
		spriteInitData.m_width = width;
		spriteInitData.m_height = height;
		// 2D用のシェーダーを使用する
		spriteInitData.m_fxFilePath = "Assets/shader/tonemap.fx";
		spriteInitData.m_vsEntryPointFunc = "VSMain";
	

		spriteInitData.m_alphaBlendMode = AlphaBlendMode_None;
	
		auto spritePtr = std::make_unique<Sprite>();
		spritePtr.get()->Init(spriteInitData);

		m_spriteVector.push_back(std::move(spritePtr));

		if (width == 1 && height == 1)
		{
			break;
		}

		number++;
	}

	//平均輝度を利用したtonemap用のスプライトを初期化する。
	SpriteInitData spriteInitData;
	spriteInitData.m_textures[0] = &m_renderTargetVector[m_renderTargetVector.size() - 1].get()->GetRenderTargetTexture();
	spriteInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32_FLOAT;
	spriteInitData.m_textures[1] = &mainRenderTarget.GetRenderTargetTexture();
	spriteInitData.m_colorBufferFormat[1] = mainRenderTarget.GetColorBufferFormat();
	
	spriteInitData.m_width = g_graphicsEngine->GetFrameBufferWidth();
	spriteInitData.m_height = g_graphicsEngine->GetFrameBufferHeight();
	// 2D用のシェーダーを使用する
	spriteInitData.m_fxFilePath = "Assets/shader/tonemap.fx";
	spriteInitData.m_vsEntryPointFunc = "VSMain";
	spriteInitData.m_psEntryPoinFunc = "FinalPSMain";

	spriteInitData.m_alphaBlendMode = AlphaBlendMode_None;

	spriteInitData.m_expandConstantBuffer = (void*)&m_tonemapBuffer;
	spriteInitData.m_expandConstantBufferSize = sizeof(TonemapBuffer) +
		(16 - (sizeof(TonemapBuffer) % 16));
	
	m_finalSprite.Init(spriteInitData);
}

void ToneMap::Render(RenderContext& rc, RenderTarget& mainRenderTarget)
{
	//レンダリングターゲットとして利用できるまで待つ。
	rc.WaitUntilToPossibleSetRenderTarget(mainRenderTarget);


	for (int i = 0; i < m_renderTargetVector.size(); i++)
	{
		// レンダリングターゲットを設定
		rc.SetRenderTargetAndViewport(*m_renderTargetVector[i].get());
		//描画。
		m_spriteVector[i].get()->Draw(rc);
		// レンダリングターゲットへの書き込み終了待ち
		rc.WaitUntilFinishDrawingToRenderTarget(*m_renderTargetVector[i].get());
		//レンダリングターゲットとして利用できるまで待つ。
		rc.WaitUntilToPossibleSetRenderTarget(*m_renderTargetVector[i].get());
	}

	// レンダリングターゲットを設定
	rc.SetRenderTargetAndViewport(mainRenderTarget);
	//描画。
	m_finalSprite.Draw(rc);
	// レンダリングターゲットへの書き込み終了待ち
	rc.WaitUntilFinishDrawingToRenderTarget(mainRenderTarget);
	
}