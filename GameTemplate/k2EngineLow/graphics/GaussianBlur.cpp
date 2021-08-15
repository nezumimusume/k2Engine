#include "k2EngineLowPreCompile.h"
#include "GaussianBlur.h"

namespace nsK2EngineLow {
	void GaussianBlur::Init(Texture* originalTexture, bool isBlurAlpha, bool isDownSample)
	{
		m_originalTexture = originalTexture;

		//�����_�����O�^�[�Q�b�g���������B
		InitRenderTargets(isDownSample);
		//�X�v���C�g���������B
		InitSprites(isBlurAlpha);
	}


	void GaussianBlur::ExecuteOnGPU(RenderContext& rc, float blurPower)
	{
		//�d�݃e�[�u�����X�V����B
		UpdateWeightsTable(blurPower);

		//���u���[�����s�B
		//�����_�����O�^�[�Q�b�g�Ƃ��ė��p�ł���悤�ɂȂ�܂�wait������B
		rc.WaitUntilToPossibleSetRenderTarget(m_xBlurRenderTarget);
		//�����_�����O�^�[�Q�b�g��ݒ�B
		rc.SetRenderTargetAndViewport(m_xBlurRenderTarget);

		//�h���[�B
		m_xBlurSprite.Draw(rc);
		//�����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂��B
		rc.WaitUntilFinishDrawingToRenderTarget(m_xBlurRenderTarget);

		//�c�u���[�����s�B
		//�����_�����O�^�[�Q�b�g�Ƃ��ė��p�ł���悤�ɂȂ�܂�wait������B
		rc.WaitUntilToPossibleSetRenderTarget(m_yBlurRenderTarget);
		//�����_�����O�^�[�Q�b�g��ݒ�B
		rc.SetRenderTargetAndViewport(m_yBlurRenderTarget);

		//�h���[�B
		m_yBlurSprite.Draw(rc);
		//�����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂��B
		rc.WaitUntilFinishDrawingToRenderTarget(m_yBlurRenderTarget);
	}

	void GaussianBlur::InitRenderTargets(bool isDownSample)
	{
		int w = m_originalTexture->GetWidth();
		int h = m_originalTexture->GetHeight();
		//X�u���[�p�̃����_�����O�^�[�Q�b�g���쐬����B
		m_xBlurRenderTarget.Create(
			isDownSample ? w / 2 : w,
			h,
			1,
			1,
			m_originalTexture->GetFormat(),
			DXGI_FORMAT_UNKNOWN
		);

		//Y�u���[�p�̃����_�����O�^�[�Q�b�g���쐬����B
		m_yBlurRenderTarget.Create(
			isDownSample ? w / 2 : w,
			isDownSample ? h / 2 : h,
			1,
			1,
			m_originalTexture->GetFormat(),
			DXGI_FORMAT_UNKNOWN
		);
	}

	void GaussianBlur::InitSprites(bool isBlurAlpha)
	{
		const char* psEntryFunc = nullptr;
		if (isBlurAlpha) {
			psEntryFunc = "PSBlur_Alpha";
		}
		else {
			psEntryFunc = "PSBlur";
		}
		//���u���[�p�̃X�v���C�g������������B
		{
			SpriteInitData xBlurSpriteInitData;
			xBlurSpriteInitData.m_fxFilePath = "Assets/shader/gaussianBlur.fx";
			xBlurSpriteInitData.m_vsEntryPointFunc = "VSXBlur";
			xBlurSpriteInitData.m_psEntryPoinFunc = psEntryFunc;
			//�X�v���C�g�̉𑜓x��m_xBlurRenderTarget�Ɠ����B
			xBlurSpriteInitData.m_width = m_xBlurRenderTarget.GetWidth();
			xBlurSpriteInitData.m_height = m_xBlurRenderTarget.GetHeight();
			//�e�N�X�`���͋P�x�e�N�X�`���B
			xBlurSpriteInitData.m_textures[0] = m_originalTexture;
			//�`�����ރ����_�����O�^�[�Q�b�g�̃t�H�[�}�b�g���w�肷��B
			xBlurSpriteInitData.m_colorBufferFormat[0] = m_xBlurRenderTarget.GetColorBufferFormat();
			//���[�U�[�g���̒萔�o�b�t�@�Ƀu���[�p�̃p�����[�^�[��ݒ肷��B
			xBlurSpriteInitData.m_expandConstantBuffer = &m_weights;
			xBlurSpriteInitData.m_expandConstantBufferSize = sizeof(m_weights);

			//�������������Ƃɉ��u���[�p�̃X�v���C�g������������B
			m_xBlurSprite.Init(xBlurSpriteInitData);
		}
		//�c�u���[�p�̃X�v���C�g������������B
		{
			SpriteInitData yBlurSpriteInitData;
			yBlurSpriteInitData.m_fxFilePath = "Assets/shader/gaussianBlur.fx";
			yBlurSpriteInitData.m_vsEntryPointFunc = "VSYBlur";
			yBlurSpriteInitData.m_psEntryPoinFunc = psEntryFunc;
			//�X�v���C�g�̉𑜓x��m_yBlurRenderTarget�Ɠ����B
			yBlurSpriteInitData.m_width = m_yBlurRenderTarget.GetWidth();
			yBlurSpriteInitData.m_height = m_yBlurRenderTarget.GetHeight();
			//�e�N�X�`���͉��u���[�����������́B
			yBlurSpriteInitData.m_textures[0] = &m_xBlurRenderTarget.GetRenderTargetTexture();
			//�`�����ރ����_�����O�^�[�Q�b�g�̃t�H�[�}�b�g���w�肷��B
			yBlurSpriteInitData.m_colorBufferFormat[0] = m_yBlurRenderTarget.GetColorBufferFormat();
			//���[�U�[�g���̒萔�o�b�t�@�Ƀu���[�p�̃p�����[�^�[��ݒ肷��B
			yBlurSpriteInitData.m_expandConstantBuffer = &m_weights;
			yBlurSpriteInitData.m_expandConstantBufferSize = sizeof(m_weights);

			//�������������Ƃɏc�u���[�p�̃X�v���C�g������������B
			m_yBlurSprite.Init(yBlurSpriteInitData);
		}
	}
	void GaussianBlur::UpdateWeightsTable(float blurPower)
	{
		float total = 0;
		for (int i = 0; i < NUM_WEIGHTS; i++) {
			m_weights[i] = expf(-0.5f * (float)(i * i) / blurPower);
			total += 2.0f * m_weights[i];

		}
		// �K�i��
		for (int i = 0; i < NUM_WEIGHTS; i++) {
			m_weights[i] /= total;
		}
	}
}