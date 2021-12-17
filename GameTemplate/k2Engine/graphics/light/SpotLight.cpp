#include "k2EnginePreCompile.h"
#include "graphics/SceneLight.h"
#include "SpotLight.h"

namespace nsK2Engine {
	SpotLight::~SpotLight()
	{
		if (g_sceneLight && m_spotLight) {
			g_sceneLight->DeleteSpotLight(m_spotLight);
		}
	}
	void SpotLight::Init()
	{
		if (m_spotLight) {
			g_sceneLight->DeleteSpotLight(m_spotLight);
		}
		m_spotLight = g_sceneLight->NewSpotLight();
	}
}