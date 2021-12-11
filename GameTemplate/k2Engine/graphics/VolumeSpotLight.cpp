#include "k2EnginePreCompile.h"
#include "VolumeSpotLight.h"

namespace nsK2Engine {
	VolumeSpotLight::VolumeSpotLight()
	{
		// シーンライトに登録。
		g_sceneLight->AddVolumeSpotLight(*this);
	}

	VolumeSpotLight::~VolumeSpotLight()
	{
		// シーンライトから削除。
		g_sceneLight->RemoveVolumeSpotLight(*this);
	}

	void VolumeSpotLight::DrawToVolumeLightMap(RenderContext& rc)
	{

	}
}