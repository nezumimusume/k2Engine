#include "k2EnginePreCompile.h"
#include "VolumeLightBase.h"

namespace nsK2Engine {
	VolumeLightBase::~VolumeLightBase()
	{
		// シーンライトから削除。
		if (g_renderingEngine) {
			g_renderingEngine->RemoveVolumeSpotLight(*this);
		}
	}
	void VolumeLightBase::Init(
		const void* lightData, 
		const char* tkmFilePath,
		const char* psFinalEntryFunc
	)
	{
		SetLightData(lightData);

		// シーンライトに登録。
		g_renderingEngine->AddVolumeSpotLight(*this);
		// ボリュームライトマップ描画用のモデル初期化設定を行う。
		// 背面描画、前面描画共通の初期化設定。
		ModelInitData modelInitData;
		modelInitData.m_tkmFilePath = tkmFilePath;
		modelInitData.m_fxFilePath = "Assets/shader/DrawVolumeLight.fx";
		modelInitData.m_expandShaderResoruceView[0] = &g_renderingEngine->GetZPrepassDepthTexture();
		// ここから前面描画用の初期化設定。
		modelInitData.m_colorBufferFormat[0] = g_drawVolumeLightMapFormat.colorBufferFormat;
		modelInitData.m_psEntryPointFunc = "PSMainFrontBack";
		modelInitData.m_cullMode = D3D12_CULL_MODE_BACK; // 前面描画なので、背面カリング。
		m_modelFront.Init(modelInitData);

		// 続いて背面描画用の初期化設定。
		// 背面描画の時にスポットライトの番号を書き込む
		modelInitData.m_colorBufferFormat[0] = g_drawVolumeLightMapFormat.colorBufferFormat;
		modelInitData.m_psEntryPointFunc = "PSMainFrontBack";
		modelInitData.m_cullMode = D3D12_CULL_MODE_FRONT; // 背面描画なので、前面カリング。
		m_modelBack.Init(modelInitData);

		// 最後に最終描画
		SpriteInitData spriteInitData;
		spriteInitData.m_fxFilePath = "Assets/shader/DrawVolumeLight.fx";
		spriteInitData.m_vsEntryPointFunc = "VSFinal";
		spriteInitData.m_psEntryPoinFunc = psFinalEntryFunc;
		spriteInitData.m_colorBufferFormat[0] = g_mainRenderTargetFormat.colorBufferFormat;
		spriteInitData.m_textures[0] = &g_renderingEngine->GetGBufferAlbedoTexture();
		spriteInitData.m_textures[1] = &g_renderingEngine->GetVolumeLightRender().GetVolumeLightMapFrontTexture();
		spriteInitData.m_textures[2] = &g_renderingEngine->GetVolumeLightRender().GetVolumeLightMapBackTexture();
		spriteInitData.m_expandConstantBuffer = GetFinalCB();
		spriteInitData.m_expandConstantBufferSize = GetFinalCBSize();

		spriteInitData.m_width = FRAME_BUFFER_W;
		spriteInitData.m_height = FRAME_BUFFER_H;
		spriteInitData.m_alphaBlendMode = AlphaBlendMode_Add;
		m_final.Init(spriteInitData);
	}
	void VolumeLightBase::DrawToVolumeLightMapBack(RenderContext& rc)
	{
		// スポットライトの番号を更新する。
		m_modelBack.Draw(rc);
	}
	void VolumeLightBase::DrawToVolumeLightMapFront(RenderContext& rc)
	{
		m_modelFront.Draw(rc);
	}
	void VolumeLightBase::DrawFinal(RenderContext& rc)
	{
		PreDrawFinal(rc);
		m_final.Draw(rc);
	}
	
}