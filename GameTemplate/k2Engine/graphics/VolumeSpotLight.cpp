#include "k2EnginePreCompile.h"
#include "VolumeSpotLight.h"

namespace nsK2Engine {
	VolumeSpotLight::VolumeSpotLight()
	{
		// �V�[�����C�g�ɓo�^�B
		g_sceneLight->AddVolumeSpotLight(*this);
	}

	VolumeSpotLight::~VolumeSpotLight()
	{
		// �V�[�����C�g����폜�B
		g_sceneLight->RemoveVolumeSpotLight(*this);
	}

	void VolumeSpotLight::DrawToVolumeLightMap(RenderContext& rc)
	{

	}
}