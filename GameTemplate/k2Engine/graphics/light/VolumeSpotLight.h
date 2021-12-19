#pragma once

#include "graphics/light/VolumeLightBase.h"

namespace nsK2Engine {
	
	/// <summary>
	/// ���f���x�[�X�̃{�����[���X�|�b�g���C�g
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
		/// �X�|�b�g���C�g�̏������ƂɍX�V
		/// </summary>
		void Update();
	};
}