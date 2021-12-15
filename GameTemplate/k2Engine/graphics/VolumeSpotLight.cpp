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

		// ここから背面描画用の初期化設定。
		modelInitData.m_colorBufferFormat[0] = g_drawVolumeLightMapBackFormat.colorBufferFormat;
		modelInitData.m_psEntryPointFunc = "PSMainBack";
		modelInitData.m_cullMode = D3D12_CULL_MODE_FRONT; // 背面描画なので、前面カリング。
		m_modelBack.Init(modelInitData);
	}
	void VolumeSpotLight::DrawToVolumeLightMapBack(RenderContext& rc)
	{
		m_modelBack.Draw(rc);
	}
	void VolumeSpotLight::DrawToVolumeLightMapFront(RenderContext& rc)
	{
		m_modelFront.Draw(rc);
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
	}
}