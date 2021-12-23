#pragma once

#include "graphics/light/VolumeLightBase.h"

namespace nsK2Engine{
    class PointLight;

    /// <summary>
    /// モデルベースのボリュームポイントライト
    /// </summary>
    class VolumePointLight : public VolumeLight<PointLight, SPointLight> {
    public:    
		void Init(PointLight& pointLight)
		{
			VolumeLightBase::Init(
				&pointLight,
				"Assets/modelData/preset/VolumePointLight.tkm",
				"PSFinal_PointLight"
			);
		}
		/// <summary>
		/// スポットライトの情報をもとに更新
		/// </summary>
		void Update();
		
    };
}