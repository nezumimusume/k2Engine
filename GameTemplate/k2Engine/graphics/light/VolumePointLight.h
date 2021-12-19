#pragma once

#include "graphics/light/VolumeLightBase.h"

namespace nsK2Engine{
    class PointLight;

    /// <summary>
    /// ���f���x�[�X�̃{�����[���|�C���g���C�g
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
		/// �X�|�b�g���C�g�̏������ƂɍX�V
		/// </summary>
		void Update();
		
    };
}