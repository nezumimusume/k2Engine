#include "k2EnginePreCompile.h"
#include "graphics/postEffect/ssr.h"

namespace nsK2Engine {
	void Ssr::OnInit(
		RenderTarget& mainRenderTarget,
		RenderTarget& depthRenderTarget,
		RenderTarget& normalRenderTarget,
		RenderTarget& metallicSmoothRenderTarget,
		RenderTarget& albedoRenderTarget
	)
	{
		{
			// �f�荞�݉摜���쐬���邽�߂̃X�v���C�g���쐬�B
			UINT w = mainRenderTarget.GetWidth();
			UINT h = mainRenderTarget.GetHeight();
			SpriteInitData initData;
			initData.m_width = w;
			initData.m_height = h;
			initData.m_fxFilePath = "Assets/shader/postEffect/ssr.fx";
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

			m_reflectionSprite.Init(initData);

			// �����f�荞�݂̋����Ƃ��Ĉ����B0.0�Ȃ�f�荞�݂Ȃ��A1.0���ł������f�荞�݂��܂��B
			// ����0.0�ŏ��������܂��B
			float clearColor[] = {
				0.0f, 0.0f, 0.0f, 0.0f
			};
			// �f�荞�݉摜��`�悷�邽�߂̃����_�����O�^�[�Q�b�g���쐬�B
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
			initData.m_fxFilePath = "Assets/shader/postEffect/ssr.fx";
			initData.m_vsEntryPointFunc = "VSMain";
			initData.m_psEntryPoinFunc = "PSFinal";

			initData.m_alphaBlendMode = AlphaBlendMode_None;
			initData.m_colorBufferFormat[0] = mainRenderTarget.GetColorBufferFormat();

			m_finalSprite.Init(initData);

			m_finalRt.Create(
				mainRenderTarget.GetWidth(),
				mainRenderTarget.GetHeight(),
				1,
				1,
				mainRenderTarget.GetColorBufferFormat(),
				DXGI_FORMAT_UNKNOWN
			);
		}

	}
	void Ssr::OnRender(RenderContext& rc, RenderTarget& mainRenderTarget)
	{
		if (!m_isEnable) {
			return;
		}
		g_graphicsEngine->BeginGPUEvent("SSR");
		// �܂��͉f�荞�݃C���[�W���쐬����B
		// �����_�����O�^�[�Q�b�g�Ƃ��ė��p�ł���܂ő҂�
		rc.WaitUntilToPossibleSetRenderTarget(m_reflectionRt);
		// �����_�����O�^�[�Q�b�g��ݒ�
		rc.SetRenderTargetAndViewport(m_reflectionRt);
		rc.ClearRenderTargetView(m_reflectionRt);

		m_ssrCb.cameraPosInWorld = g_camera3D->GetPosition();
		m_ssrCb.mViewProj = g_camera3D->GetViewProjectionMatrix();
		m_ssrCb.mViewProjInv.Inverse(m_ssrCb.mViewProj);

		// �`��
		m_reflectionSprite.Draw(rc);
		// �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
		rc.WaitUntilFinishDrawingToRenderTarget(m_reflectionRt);

		// �f�荞�݉摜���K�E�V�A���u���[�łڂ����ăf�m�C�Y���s���B
		m_blur.ExecuteOnGPU(rc, 2.0f);

		// �����_�����O�^�[�Q�b�g�Ƃ��ė��p�ł���܂ő҂�
		rc.WaitUntilToPossibleSetRenderTarget(m_finalRt);
		// �����_�����O�^�[�Q�b�g��ݒ�
		rc.SetRenderTargetAndViewport(m_finalRt);
		// �f�荞�݉摜�ƃ��C���V�[�����������Ă����B
		m_finalSprite.Draw(rc);
		// �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
		rc.WaitUntilFinishDrawingToRenderTarget(m_finalRt);
		g_graphicsEngine->EndGPUEvent();
	}
}