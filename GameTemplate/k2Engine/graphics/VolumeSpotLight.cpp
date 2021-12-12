#include "k2EnginePreCompile.h"
#include "VolumeSpotLight.h"

namespace nsK2Engine {
	VolumeSpotLight::VolumeSpotLight()
	{
		
	}

	VolumeSpotLight::~VolumeSpotLight()
	{
		// シーンライトから削除。
		g_sceneLight->RemoveVolumeSpotLight(*this);
	}
	void VolumeSpotLight::Init()
	{
		// シーンライトに登録。
		g_sceneLight->AddVolumeSpotLight(*this);
		// 続きはここから
		ModelInitData modelInitData;
		modelInitData.m_tkmFilePath = "Assets/modelData/preset/VolumeSpotLight.tkm";
		modelInitData.m_fxFilePath = "Assets/shader/DrawVolumeLightMap.fx";
		modelInitData.m_colorBufferFormat[0] = g_drawVolumeLightMapFormat.colorBufferFormat;
		m_modelFront.Init(modelInitData);

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