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
	void VolumeSpotLight::Init(const SpotLight& spotLight)
	{
		m_spotLight = &spotLight;
		// �V�[�����C�g�ɓo�^�B
		g_renderingEngine->AddVolumeSpotLight(*this);
		// �{�����[�����C�g�}�b�v�`��p�̃��f���������ݒ���s���B
		// �w�ʕ`��A�O�ʕ`�拤�ʂ̏������ݒ�B
		ModelInitData modelInitData;
		modelInitData.m_tkmFilePath = "Assets/modelData/preset/VolumeSpotLight.tkm";
		modelInitData.m_fxFilePath = "Assets/shader/DrawVolumeLight.fx";
		modelInitData.m_expandShaderResoruceView[0] = &g_renderingEngine->GetZPrepassDepthTexture();
		// ��������O�ʕ`��p�̏������ݒ�B
		modelInitData.m_colorBufferFormat[0] = g_drawVolumeLightMapFormat.colorBufferFormat;
		modelInitData.m_psEntryPointFunc = "PSMainFrontBack";
		modelInitData.m_cullMode = D3D12_CULL_MODE_BACK; // �O�ʕ`��Ȃ̂ŁA�w�ʃJ�����O�B
		m_modelFront.Init(modelInitData);

		// �����Ĕw�ʕ`��p�̏������ݒ�B
		// �w�ʕ`��̎��ɃX�|�b�g���C�g�̔ԍ�����������
		modelInitData.m_colorBufferFormat[0] = g_drawVolumeLightMapFormat.colorBufferFormat;
		modelInitData.m_psEntryPointFunc = "PSMainFrontBack";
		modelInitData.m_cullMode = D3D12_CULL_MODE_FRONT; // �w�ʕ`��Ȃ̂ŁA�O�ʃJ�����O�B
		m_modelBack.Init(modelInitData);

		// �[�x�l�������ݗp�̃��f�����������B
		modelInitData.m_colorBufferFormat[0] = g_mainRenderTargetFormat.colorBufferFormat;
		modelInitData.m_psEntryPointFunc = "PSDepth";
		modelInitData.m_cullMode = D3D12_CULL_MODE_BACK;
		modelInitData.m_alphaBlendMode = AlphaBlendMode_Trans;
		m_drawDepthModel.Init(modelInitData);

		// �Ō�ɍŏI�`��
		SpriteInitData spriteInitData;
		spriteInitData.m_fxFilePath = "Assets/shader/DrawVolumeLight.fx";
		spriteInitData.m_vsEntryPointFunc = "VSFinal";
		spriteInitData.m_psEntryPoinFunc = "PSFinal";
		spriteInitData.m_colorBufferFormat[0] = g_mainRenderTargetFormat.colorBufferFormat;
		spriteInitData.m_textures[0] = &g_renderingEngine->GetGBufferAlbedoTexture();
		spriteInitData.m_textures[1] = &g_renderingEngine->GetVolumeLightRender().GetVolumeLightMapFrontTexture();
		spriteInitData.m_textures[2] = &g_renderingEngine->GetVolumeLightRender().GetVolumeLightMapBackTexture();
		spriteInitData.m_expandConstantBuffer = &m_finalCB;
		spriteInitData.m_expandConstantBufferSize = sizeof(m_finalCB);
		
		spriteInitData.m_width = FRAME_BUFFER_W;
		spriteInitData.m_height = FRAME_BUFFER_H;
		spriteInitData.m_alphaBlendMode = AlphaBlendMode_Add;
		m_final.Init(spriteInitData);

		

		
	}
	void VolumeSpotLight::DrawToVolumeLightMapBack(RenderContext& rc)
	{
		// �X�|�b�g���C�g�̔ԍ����X�V����B
		m_modelBack.Draw(rc);
	}
	void VolumeSpotLight::DrawToVolumeLightMapFront(RenderContext& rc)
	{
		m_modelFront.Draw(rc);
	}
	void VolumeSpotLight::DrawFinal(RenderContext& rc)
	{
		// �ŏI�`��̒萔�o�b�t�@���X�V����B
		m_finalCB.spotLight = m_spotLight->GetRawData();
		// �J�����֌W�̃f�[�^��Draw�̃p�X�ŃR�s�[���Ȃ��ƁAUpdate�̏��Ԃ�
		// 1�t���[���O�̃J�������ɂȂ��Ă��܂��B
		m_finalCB.mViewProjInv = g_camera3D->GetViewProjectionMatrixInv();

		m_final.Draw(rc);
	}
	void VolumeSpotLight::DrawDepth(RenderContext& rc)
	{
		m_drawDepthModel.Draw(rc);
	}
	void VolumeSpotLight::Update()
	{
		// �X�|�b�g���C�g�̏������ƂɃ��[���h�s����X�V����B
		Vector3 pos = m_spotLight->GetPosition();
		Quaternion rot;
		rot.SetRotation(g_vec3Back, m_spotLight->GetDirection());
		
		Vector3 scale;
		scale.z = m_spotLight->GetRange();
		float angle = m_spotLight->GetAngle();
		// �p�x����xy�̊g�嗦���v�Z����B
		float xyScale = tan(angle) * m_spotLight->GetRange() ;
		scale.x = xyScale;
		scale.y = xyScale;
		m_modelBack.UpdateWorldMatrix(pos, rot, scale);
		m_modelFront.UpdateWorldMatrix(pos, rot, scale);
		m_drawDepthModel.UpdateWorldMatrix(pos, rot, scale);
	}
}