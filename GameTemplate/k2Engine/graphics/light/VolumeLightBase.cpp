#include "k2EnginePreCompile.h"
#include "VolumeLightBase.h"

namespace nsK2Engine {
	VolumeLightBase::~VolumeLightBase()
	{
		// �V�[�����C�g����폜�B
		if (g_renderingEngine) {
			g_renderingEngine->RemoveVolumeSpotLight(*this);
		}
	}
	void VolumeLightBase::Init(
		const void* lightData, 
		const char* tkmFilePath,
		const char* psFinalEntryFunc
	)
	{
		SetLightData(lightData);

		// �V�[�����C�g�ɓo�^�B
		g_renderingEngine->AddVolumeSpotLight(*this);
		// �{�����[�����C�g�}�b�v�`��p�̃��f���������ݒ���s���B
		// �w�ʕ`��A�O�ʕ`�拤�ʂ̏������ݒ�B
		ModelInitData modelInitData;
		modelInitData.m_tkmFilePath = tkmFilePath;
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

		// �Ō�ɍŏI�`��
		SpriteInitData spriteInitData;
		spriteInitData.m_fxFilePath = "Assets/shader/DrawVolumeLight.fx";
		spriteInitData.m_vsEntryPointFunc = "VSFinal";
		spriteInitData.m_psEntryPoinFunc = psFinalEntryFunc;
		spriteInitData.m_colorBufferFormat[0] = g_mainRenderTargetFormat.colorBufferFormat;
		spriteInitData.m_textures[0] = &g_renderingEngine->GetGBufferAlbedoTexture();
		spriteInitData.m_textures[1] = &g_renderingEngine->GetVolumeLightRender().GetVolumeLightMapFrontTexture();
		spriteInitData.m_textures[2] = &g_renderingEngine->GetVolumeLightRender().GetVolumeLightMapBackTexture();
		spriteInitData.m_expandConstantBuffer = GetFinalCB();
		spriteInitData.m_expandConstantBufferSize = GetFinalCBSize();

		spriteInitData.m_width = FRAME_BUFFER_W;
		spriteInitData.m_height = FRAME_BUFFER_H;
		spriteInitData.m_alphaBlendMode = AlphaBlendMode_Add;
		m_final.Init(spriteInitData);
	}
	void VolumeLightBase::DrawToVolumeLightMapBack(RenderContext& rc)
	{
		// �X�|�b�g���C�g�̔ԍ����X�V����B
		m_modelBack.Draw(rc);
	}
	void VolumeLightBase::DrawToVolumeLightMapFront(RenderContext& rc)
	{
		m_modelFront.Draw(rc);
	}
	void VolumeLightBase::DrawFinal(RenderContext& rc)
	{
		PreDrawFinal(rc);
		m_final.Draw(rc);
	}
	
}