#include "k2EnginePreCompile.h"
#include "VolumeSpotLight.h"

namespace nsK2Engine {
	VolumeSpotLight::VolumeSpotLight()
	{
		
	}

	VolumeSpotLight::~VolumeSpotLight()
	{
		// �V�[�����C�g����폜�B
		if (g_renderingEngine) {
			g_renderingEngine->RemoveVolumeSpotLight(*this);
		}
	}
	void VolumeSpotLight::Init()
	{
		// �V�[�����C�g�ɓo�^�B
		g_renderingEngine->AddVolumeSpotLight(*this);
		// �{�����[�����C�g�}�b�v�`��p�̃��f���������ݒ���s���B
		// �w�ʕ`��A�O�ʕ`�拤�ʂ̏������ݒ�B
		ModelInitData modelInitData;
		modelInitData.m_tkmFilePath = "Assets/modelData/preset/VolumeSpotLight.tkm";
		modelInitData.m_fxFilePath = "Assets/shader/DrawVolumeLightMap.fx";
		modelInitData.m_expandShaderResoruceView[0] = &g_renderingEngine->GetZPrepassDepthTexture();

		// ��������O�ʕ`��p�̏������ݒ�B
		modelInitData.m_colorBufferFormat[0] = g_drawVolumeLightMapFrontFormat.colorBufferFormat;
		modelInitData.m_psEntryPointFunc = "PSMainFront";
		modelInitData.m_cullMode = D3D12_CULL_MODE_BACK; // �O�ʕ`��Ȃ̂ŁA�w�ʃJ�����O�B
		m_modelFront.Init(modelInitData);

		// ��������w�ʕ`��p�̏������ݒ�B
		modelInitData.m_colorBufferFormat[0] = g_drawVolumeLightMapBackFormat.colorBufferFormat;
		modelInitData.m_psEntryPointFunc = "PSMainBack";
		modelInitData.m_cullMode = D3D12_CULL_MODE_FRONT; // �w�ʕ`��Ȃ̂ŁA�O�ʃJ�����O�B
		m_modelBack.Init(modelInitData);
	}
	void VolumeSpotLight::DrawToVolumeLightMapBack(RenderContext& rc)
	{
		m_modelBack.Draw(rc);
	}
	void VolumeSpotLight::DrawToVolumeLightMapFront(RenderContext& rc)
	{
		m_modelFront.Draw(rc);
	}
	
	void VolumeSpotLight::Update(const SpotLight& spotLight)
	{
		
		// �X�|�b�g���C�g�̏������ƂɃ��[���h�s����X�V����B
		Vector3 pos = spotLight.GetPosition();
		Quaternion rot;
		rot.SetRotation(g_vec3Back, spotLight.GetDirection());
		
		Vector3 scale;
		scale.z = spotLight.GetRange();
		float angle = spotLight.GetAngle();
		// �p�x����xy�̊g�嗦���v�Z����B
		float xyScale = tan(angle) * spotLight.GetRange() ;
		scale.x = xyScale;
		scale.y = xyScale;
		m_modelBack.UpdateWorldMatrix(pos, rot, scale);
		m_modelFront.UpdateWorldMatrix(pos, rot, scale);
	}
}