#include "k2EnginePreCompile.h"
#include "VolumePointLight.h"

namespace nsK2Engine {
	
	void VolumePointLight::Update()
	{
		// ポイントライトの情報をもとにワールド行列を更新する。
		Vector3 pos = m_lightData->GetPosition();
		Vector3 scale;
		scale.x = m_lightData->GetRange();
		scale.y = m_lightData->GetRange();
		scale.z = m_lightData->GetRange();

		m_modelBack.UpdateWorldMatrix(pos, g_quatIdentity, scale);
		m_modelFront.UpdateWorldMatrix(pos, g_quatIdentity, scale);
	}
}