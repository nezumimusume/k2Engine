#pragma once

#include "graphics/light/VolumeLightBase.h"

namespace nsK2Engine {
	
	/// <summary>
	/// モデルベースのボリュームスポットライト
	/// </summary>
	class VolumeSpotLight : public VolumeLight<SpotLight, SSpotLight> {
	public:
		void Init(SpotLight& spotLight)
		{
			VolumeLightBase::Init(
				&spotLight,
				"Assets/modelData/preset/VolumeSpotLight.tkm",
				"PSFinal_SpotLight"
			);
		}
		/// <summary>
		/// スポットライトの情報をもとに更新
		/// </summary>
		void Update();
	};
}