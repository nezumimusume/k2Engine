#include "k2EnginePreCompile.h"
#include "VolumeSpotLight.h"

namespace nsK2Engine {
	VolumeSpotLight::VolumeSpotLight()
	{
		
	}

	VolumeSpotLight::~VolumeSpotLight()
	{
		// シーンライトから削除。
		if (g_renderingEngine) {
			g_renderingEngine->RemoveVolumeSpotLight(*this);
		}
	}
	void VolumeSpotLight::Init()
	{
		// シーンライトに登録。
		g_renderingEngine->AddVolumeSpotLight(*this);
		// ボリュームライトマップ描画用のモデル初期化設定を行う。
		// 背面描画、前面描画共通の初期化設定。
		ModelInitData modelInitData;
		modelInitData.m_tkmFilePath = "Assets/modelData/preset/VolumeSpotLight.tkm";
		modelInitData.m_fxFilePath = "Assets/shader/DrawVolumeLightMap.fx";
		modelInitData.m_expandShaderResoruceView[0] = &g_renderingEngine->GetZPrepassDepthTexture();
		// ここから前面描画用の初期化設定。
		modelInitData.m_colorBufferFormat[0] = g_drawVolumeLightMapFrontFormat.colorBufferFormat;
		modelInitData.m_psEntryPointFunc = "PSMainFront";
		modelInitData.m_cullMode = D3D12_CULL_MODE_BACK; // 前面描画なので、背面カリング。
		m_modelFront.Init(modelInitData);

		// 続いて背面描画用の初期化設定。
		// 背面描画の時にスポットライトの番号を書き込む
		modelInitData.m_expandConstantBuffer = &m_spotLightInfo;
		modelInitData.m_expandConstantBufferSize = sizeof(m_spotLightInfo);
		modelInitData.m_colorBufferFormat[0] = g_drawVolumeLightMapBackFormat.colorBufferFormat;
		modelInitData.m_psEntryPointFunc = "PSMainBack";
		modelInitData.m_cullMode = D3D12_CULL_MODE_FRONT; // 背面描画なので、前面カリング。
		m_modelBack.Init(modelInitData);

		// 最後に最終描画
		SpriteInitData spriteInitData;
		spriteInitData.m_fxFilePath = 
		modelInitData.m_cullMode = D3D12_CULL_MODE_NONE; // 背面描画なので、前面カリング。
		auto& volumeLightRender = g_renderingEngine->GetVolumeLightRender();
		modelInitData.m_colorBufferFormat[0] = g_mainRenderTargetFormat.colorBufferFormat;		
		modelInitData.m_expandConstantBuffer = &g_renderingEngine->GetDeferredLightingCB().m_light;
		modelInitData.m_expandConstantBufferSize = sizeof(g_renderingEngine->GetDeferredLightingCB().m_light);
		modelInitData.m_expandShaderResoruceView[0] = &volumeLightRender.GetVolumeLightMapFrontTexture();
		modelInitData.m_expandShaderResoruceView[1] = &volumeLightRender.GetVolumeLightMapBackTexture();
		modelInitData.m_psEntryPointFunc = "PSMainFinal";
		modelInitData.m_alphaBlendMode = AlphaBlendMode_Add;
		modelInitData.m_isDepthWrite = false;
		m_modelFinal.Init(modelInitData);
	}
	void VolumeSpotLight::DrawToVolumeLightMapBack(RenderContext& rc)
	{
		m_modelBack.Draw(rc);
	}
	void VolumeSpotLight::DrawToVolumeLightMapFront(RenderContext& rc)
	{
		m_modelFront.Draw(rc);
	}
	void VolumeSpotLight::DrawFinal(RenderContext& rc)
	{
		m_modelFinal.Draw(rc);
	}
	void VolumeSpotLight::Update(const SpotLight& spotLight)
	{
		
		// スポットライトの情報をもとにワールド行列を更新する。
		Vector3 pos = spotLight.GetPosition();
		Quaternion rot;
		rot.SetRotation(g_vec3Back, spotLight.GetDirection());
		
		Vector3 scale;
		scale.z = spotLight.GetRange();
		float angle = spotLight.GetAngle();
		// 角度からxyの拡大率を計算する。
		float xyScale = tan(angle) * spotLight.GetRange() ;
		scale.x = xyScale;
		scale.y = xyScale;
		m_modelBack.UpdateWorldMatrix(pos, rot, scale);
		m_modelFront.UpdateWorldMatrix(pos, rot, scale);
		m_modelFinal.UpdateWorldMatrix(pos, rot, scale);
		// スポットライトの番号を更新する。
		m_spotLightInfo.no = spotLight.GetNo();
	}
}