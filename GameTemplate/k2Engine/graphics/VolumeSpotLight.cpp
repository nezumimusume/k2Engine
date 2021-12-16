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

		// �����Ĕw�ʕ`��p�̏������ݒ�B
		// �w�ʕ`��̎��ɃX�|�b�g���C�g�̔ԍ�����������
		modelInitData.m_expandConstantBuffer = &m_spotLightInfo;
		modelInitData.m_expandConstantBufferSize = sizeof(m_spotLightInfo);
		modelInitData.m_colorBufferFormat[0] = g_drawVolumeLightMapBackFormat.colorBufferFormat;
		modelInitData.m_psEntryPointFunc = "PSMainBack";
		modelInitData.m_cullMode = D3D12_CULL_MODE_FRONT; // �w�ʕ`��Ȃ̂ŁA�O�ʃJ�����O�B
		m_modelBack.Init(modelInitData);

		// �Ō�ɍŏI�`��
		SpriteInitData spriteInitData;
		spriteInitData.m_fxFilePath = "Assets/shader/DrawVolumeLightMap.fx";
		spriteInitData.m_vsEntryPointFunc = "VSFinal";
		spriteInitData.m_psEntryPoinFunc = "PSFinal";
		spriteInitData.m_colorBufferFormat[0] = g_mainRenderTargetFormat.colorBufferFormat;
		spriteInitData.m_textures[0] = &g_renderingEngine->GetGBufferAlbedoTexture();
		spriteInitData.m_textures[1] = &g_renderingEngine->GetVolumeLightRender().GetVolumeLightMapFrontTexture();
		spriteInitData.m_textures[2] = &g_renderingEngine->GetVolumeLightRender().GetVolumeLightMapBackTexture();
		spriteInitData.m_expandConstantBuffer = &g_renderingEngine->GetDeferredLightingCB().m_light;
		spriteInitData.m_expandConstantBufferSize = sizeof(g_renderingEngine->GetDeferredLightingCB().m_light);
		
		spriteInitData.m_width = FRAME_BUFFER_W;
		spriteInitData.m_height = FRAME_BUFFER_H;
		spriteInitData.m_alphaBlendMode = AlphaBlendMode_Add;
		m_final.Init(spriteInitData);
		
	}
	void VolumeSpotLight::DrawToVolumeLightMapBack(RenderContext& rc)
	{
		m_modelBack.Draw(rc);
	}
	void VolumeSpotLight::DrawToVolumeLightMapFront(RenderContext& rc)
	{
		m_modelFront.Draw(rc);
	}
	void VolumeSpotLight::DrawFinal(RenderContext& rc)
	{
		m_final.Draw(rc);
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
		
		// �X�|�b�g���C�g�̔ԍ����X�V����B
		m_spotLightInfo.no = spotLight.GetNo();
	}
}