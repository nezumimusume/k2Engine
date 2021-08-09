#include "k2EnginePreCompile.h"
#include "graphics/ssr.h"

void Ssr::Init(
	RenderTarget& mainRenderTarget, 
	RenderTarget& depthRenderTarget,
	RenderTarget& normalRenderTarget,
	RenderTarget& metallicSmoothRenderTarget,
	RenderTarget& albedoRenderTarget
)
{
	{
		UINT w = mainRenderTarget.GetWidth();
		UINT h = mainRenderTarget.GetHeight();
		SpriteInitData initData;
		initData.m_width = w;
		initData.m_height = h;
		initData.m_fxFilePath = "Assets/shader/ssr.fx";
		initData.m_vsEntryPointFunc = "VSMain";
		initData.m_psEntryPoinFunc = "PSMain";
		initData.m_textures[0] = &mainRenderTarget.GetRenderTargetTexture();
		initData.m_textures[1] = &depthRenderTarget.GetRenderTargetTexture();
		initData.m_textures[2] = &normalRenderTarget.GetRenderTargetTexture();
		initData.m_textures[3] = &metallicSmoothRenderTarget.GetRenderTargetTexture();
		initData.m_expandConstantBuffer = &m_ssrCb;
		initData.m_expandConstantBufferSize = sizeof(SsrCb);

		initData.m_alphaBlendMode = AlphaBlendMode_None;
		initData.m_colorBufferFormat[0] = mainRenderTarget.GetColorBufferFormat();
		initData.textureAddressMode = D3D12_TEXTURE_ADDRESS_MODE_BORDER;

		m_sprite.Init(initData);

		float clearColor[] = {
			0.0f, 0.0f, 0.0f, 0.0f
		};
		
		m_reflectionRt.Create(
			w,
			h,
			1,
			1,
			mainRenderTarget.GetColorBufferFormat(),
			DXGI_FORMAT_UNKNOWN,
			clearColor
		);
	}
	m_blur.Init(&m_reflectionRt.GetRenderTargetTexture(), true);
	{
		// �ŏI�����p�̃X�v���C�g������������
		SpriteInitData initData;
		initData.m_textures[0] = &mainRenderTarget.GetRenderTargetTexture();
		initData.m_textures[1] = &m_blur.GetBokeTexture();
		initData.m_textures[2] = &albedoRenderTarget.GetRenderTargetTexture();
		
		// �𑜓x��mainRenderTarget�̕��ƍ���
		initData.m_width = mainRenderTarget.GetWidth();
		initData.m_height = mainRenderTarget.GetHeight();
		// 2D�p�̃V�F�[�_�[���g�p����
		initData.m_fxFilePath = "Assets/shader/ssr.fx";
		initData.m_vsEntryPointFunc = "VSMain";
		initData.m_psEntryPoinFunc = "PSFinal";

		initData.m_alphaBlendMode = AlphaBlendMode_None;
		initData.m_colorBufferFormat[0] = mainRenderTarget.GetColorBufferFormat();
		
		m_finalSprite.Init(initData);
	}

}
void Ssr::Render(RenderContext& rc, RenderTarget& mainRenderTarget)
{
	// �����_�����O�^�[�Q�b�g�Ƃ��ė��p�ł���܂ő҂�
	rc.WaitUntilToPossibleSetRenderTarget(m_reflectionRt);
	// �����_�����O�^�[�Q�b�g��ݒ�
	rc.SetRenderTargetAndViewport(m_reflectionRt);
	rc.ClearRenderTargetView(m_reflectionRt);

	m_ssrCb.cameraPosInWorld = g_camera3D->GetPosition();
	m_ssrCb.mViewProj = g_camera3D->GetViewProjectionMatrix();
	m_ssrCb.mViewProjInv.Inverse(m_ssrCb.mViewProj);
	
	
	// �`��
	m_sprite.Draw(rc);
	// �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
	rc.WaitUntilFinishDrawingToRenderTarget(m_reflectionRt);
	
	m_blur.ExecuteOnGPU(rc, 2.0f);

	// �����_�����O�^�[�Q�b�g�Ƃ��ė��p�ł���܂ő҂�
	rc.WaitUntilToPossibleSetRenderTarget(mainRenderTarget);
	// �����_�����O�^�[�Q�b�g��ݒ�
	rc.SetRenderTargetAndViewport(mainRenderTarget);

	m_finalSprite.Draw(rc);
	// �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
	rc.WaitUntilFinishDrawingToRenderTarget(mainRenderTarget);
}