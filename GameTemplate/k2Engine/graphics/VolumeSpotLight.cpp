#include "k2EnginePreCompile.h"
#include "VolumeSpotLight.h"

namespace nsK2Engine {
	VolumeSpotLight::VolumeSpotLight()
	{
		
	}

	VolumeSpotLight::~VolumeSpotLight()
	{
		// �V�[�����C�g����폜�B
		g_sceneLight->RemoveVolumeSpotLight(*this);
	}
	void VolumeSpotLight::Init()
	{
		// �V�[�����C�g�ɓo�^�B
		g_sceneLight->AddVolumeSpotLight(*this);
		// �����͂�������
	}
	void VolumeSpotLight::DrawToVolumeLightMap(RenderContext& rc)
	{

	}
}