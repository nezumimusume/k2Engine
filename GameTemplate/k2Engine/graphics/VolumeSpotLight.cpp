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
	}
	void VolumeSpotLight::DrawToVolumeLightMap(RenderContext& rc)
	{

	}
}