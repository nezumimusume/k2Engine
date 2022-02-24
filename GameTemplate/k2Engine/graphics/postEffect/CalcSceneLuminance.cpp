#include "k2EnginePreCompile.h"
#include "graphics/postEffect/CalcSceneLuminance.h"

namespace nsK2Engine {
	namespace {
		//-----------------------------------------------------------------------------
		// Name: GetSampleOffsets_DownScale4x4
		// Desc: Get the texture coordinate offsets to be used inside the DownScale4x4
		//       pixel shader.
		//-----------------------------------------------------------------------------
		HRESULT GetSampleOffsets_DownScale4x4(DWORD dwWidth, DWORD dwHeight, Vector4 avSampleOffsets[])
		{
			if (NULL == avSampleOffsets)
				return E_INVALIDARG;

			float tU = 1.0f / dwWidth;
			float tV = 1.0f / dwHeight;

			// Sample from the 16 surrounding points. Since the center point will be in
			// the exact center of 16 texels, a 0.5f offset is needed to specify a texel
			// center.
			int index = 0;
			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 4; x++)
				{
					avSampleOffsets[index].x = (x - 1.5f) * tU;
					avSampleOffsets[index].y = (y - 1.5f) * tV;

					index++;
				}
			}

			return S_OK;
		}
	}

	void CalcSceneLuminance::OnInit(
		RenderTarget& mainRenderTarget,
		RenderTarget& zprepassRenderTarget,
		RenderTarget& normalRenderTarget,
		RenderTarget& metallicSmoothRenderTarget,
		RenderTarget& albedoRenderTarget
	) {
		// ���ϋP�x�v�Z�p�̃����_�����O�^�[�Q�b�g���쐬�B
		for (int i = 0; i < enNumCalcAvgSprite; i++) {
			int rtSize = 1 << (2 * (enNumCalcAvgSprite - i - 1));
			m_calcAvgRt[i].Create(
				rtSize,
				rtSize,
				1,
				1,
				DXGI_FORMAT_R16_FLOAT,
				DXGI_FORMAT_UNKNOWN
			);
		}
		// �ŏI�I�Ƀg�[���}�b�v�Ŏg�p���镽�ϋP�x���������ރ����_�����O�^�[�Q�b�g���쐬�B
		m_luminanceAvgInTonemapRt.Create(
			1,
			1,
			1,
			1,
			DXGI_FORMAT_R16_FLOAT,
			DXGI_FORMAT_UNKNOWN
		);
		// 1�t���[���O�̕��ϋP�x���������ރ����_�����O�^�[�Q�b�g���쐬�B
		m_luminanceAvgInTonemapLastFrameRt.Create(
			1,
			1,
			1,
			1,
			DXGI_FORMAT_R16_FLOAT,
			DXGI_FORMAT_UNKNOWN
		);

		m_cb1.deltaTime = 1.0f / 60.0f;
		// �ΐ����ς��Ƃ�X�v���C�g���������B
		int curRtNo = 0;
		{
			SpriteInitData initData;
			initData.m_width = m_calcAvgRt[curRtNo].GetWidth();
			initData.m_height = m_calcAvgRt[curRtNo].GetHeight();
			initData.m_colorBufferFormat[0] = m_calcAvgRt[curRtNo].GetColorBufferFormat();
			initData.m_fxFilePath = "Assets/shader/postEffect/CalcSceneLuminance.fx";
			initData.m_psEntryPoinFunc = "PSCalcLuminanceLogAvarage";
			initData.m_expandConstantBuffer = m_avSampleOffsets;
			initData.m_expandConstantBufferSize = sizeof(m_avSampleOffsets);
			initData.m_textures[0] = &mainRenderTarget.GetRenderTargetTexture();
			m_calcAvgSprites[enCalcAvgLog].Init(initData);
		}
		// ���ς��Ƃ�X�v���C�g���������B
		curRtNo++;
		int calsAvgSpriteNo = enCalcAvg_Start;
		while (curRtNo < enCalcAvg_End) {
			SpriteInitData initData;
			initData.m_width = m_calcAvgRt[curRtNo].GetWidth();
			initData.m_height = m_calcAvgRt[curRtNo].GetHeight();
			initData.m_colorBufferFormat[0] = m_calcAvgRt[curRtNo].GetColorBufferFormat();
			initData.m_fxFilePath = "Assets/shader/postEffect/CalcSceneLuminance.fx";
			initData.m_psEntryPoinFunc = "PSCalcLuminanceAvarage";
			initData.m_expandConstantBuffer = m_avSampleOffsets;
			initData.m_expandConstantBufferSize = sizeof(m_avSampleOffsets);
			initData.m_textures[0] = &m_calcAvgRt[curRtNo - 1].GetRenderTargetTexture();
			m_calcAvgSprites[calsAvgSpriteNo].Init(initData);
			calsAvgSpriteNo++;
			curRtNo++;
		}
		// exp�֐���p���čŏI���ς����߂�B
		{
			SpriteInitData initData;
			initData.m_width = m_calcAvgRt[curRtNo].GetWidth();
			initData.m_height = m_calcAvgRt[curRtNo].GetHeight();
			initData.m_colorBufferFormat[0] = m_calcAvgRt[curRtNo].GetColorBufferFormat();
			initData.m_fxFilePath = "Assets/shader/postEffect/CalcSceneLuminance.fx";
			initData.m_psEntryPoinFunc = "PSCalcLuminanceExpAvarage";
			initData.m_expandConstantBuffer = m_avSampleOffsets;
			initData.m_expandConstantBufferSize = sizeof(m_avSampleOffsets);
			initData.m_textures[0] = &m_calcAvgRt[curRtNo - 1].GetRenderTargetTexture();
			m_calcAvgSprites[curRtNo].Init(initData);
		}
		// ���Ï���
		{
			SpriteInitData initData;
			initData.m_width = 1;
			initData.m_height = 1;
			initData.m_colorBufferFormat[0] = m_calcAvgRt[enCalcAvgExp].GetColorBufferFormat();
			initData.m_fxFilePath = "Assets/shader/postEffect/CalcSceneLuminance.fx";
			initData.m_psEntryPoinFunc = "PSCalcAdaptedLuminance";
			initData.m_expandConstantBuffer = &m_cb1;
			initData.m_expandConstantBufferSize = sizeof(m_cb1);
			initData.m_textures[0] = &m_calcAvgRt[enCalcAvgExp].GetRenderTargetTexture();
			initData.m_textures[1] = &m_luminanceAvgInTonemapLastFrameRt.GetRenderTargetTexture();

			m_calcAdapteredLuminanceSprite.Init(initData);
		}
		{
			SpriteInitData initData;
			initData.m_width = 1;
			initData.m_height = 1;
			initData.m_colorBufferFormat[0] = m_calcAvgRt[enCalcAvgExp].GetColorBufferFormat();
			initData.m_fxFilePath = "Assets/shader/postEffect/CalcSceneLuminance.fx";
			initData.m_psEntryPoinFunc = "PSCalcAdaptedLuminanceFirst";
			initData.m_expandConstantBuffer = &m_cb1;
			initData.m_expandConstantBufferSize = sizeof(m_cb1);
			initData.m_textures[0] = &m_calcAvgRt[enCalcAvgExp].GetRenderTargetTexture();

			m_calcAdapteredLuminanceFisrtSprite.Init(initData);
		}

		{
			SpriteInitData initData;
			initData.m_width = m_luminanceAvgInTonemapRt.GetWidth();
			initData.m_height = m_luminanceAvgInTonemapRt.GetHeight();
			initData.m_colorBufferFormat[0] = m_luminanceAvgInTonemapRt.GetColorBufferFormat();
			initData.m_fxFilePath = "Assets/shader/sprite.fx";
			initData.m_textures[0] = &m_luminanceAvgInTonemapRt.GetRenderTargetTexture();
			m_copyLuminanceAvgInTonemapSprite.Init(initData);
		}
	}
	void CalcSceneLuminance::CalcLuminanceAvarage(RenderContext& rc)
	{
		// �V�[���̋P�x�̕��ς��v�Z���Ă����B
		for (int spriteNo = 0; spriteNo < enNumCalcAvgSprite; spriteNo++) {

			// �����_�����O�^�[�Q�b�g�Ƃ��ė��p�ł���܂ő҂�
			rc.WaitUntilToPossibleSetRenderTarget(m_calcAvgRt[spriteNo]);
			// �����_�����O�^�[�Q�b�g��ݒ�
			rc.SetRenderTargetAndViewport(m_calcAvgRt[spriteNo]);
			rc.ClearRenderTargetView(m_calcAvgRt[spriteNo]);

			GetSampleOffsets_DownScale4x4(
				m_calcAvgSprites[spriteNo].GetTextureWidth(0),
				m_calcAvgSprites[spriteNo].GetTextureHeight(0),
				m_avSampleOffsets
			);

			m_calcAvgSprites[spriteNo].Draw(rc);

			// �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
			rc.WaitUntilFinishDrawingToRenderTarget(m_calcAvgRt[spriteNo]);
		}
	}
	void CalcSceneLuminance::OnRender(RenderContext& rc, RenderTarget& mainRenderTarget)
	{
		g_graphicsEngine->BeginGPUEvent("CalcSceneLuminance");

		CalcLuminanceAvarage(rc);

		// ���Ï���
		m_cb1.deltaTime = g_gameTime->GetFrameDeltaTime();
		rc.WaitUntilToPossibleSetRenderTarget(m_luminanceAvgInTonemapRt);
		// �����_�����O�^�[�Q�b�g��ݒ�
		rc.SetRenderTargetAndViewport(m_luminanceAvgInTonemapRt);

		if (m_isFirstWhenChangeScene) {
			m_changeSceneTimer -= g_gameTime->GetFrameDeltaTime();
			if (m_changeSceneTimer > 0.0f) {
				// �V�[���؂�ւ��I���B
				m_isFirstWhenChangeScene = false;
			}
			m_calcAdapteredLuminanceFisrtSprite.Draw(rc);
		}
		else {
			m_calcAdapteredLuminanceSprite.Draw(rc);
		}

		// �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
		rc.WaitUntilFinishDrawingToRenderTarget(m_luminanceAvgInTonemapRt);

		
		// �g�[���}�b�v�Ŏg�p�������ϋP�x��ۑ�����B
		// �����_�����O�^�[�Q�b�g���g�p�\�ɂȂ�܂ő҂B
		rc.WaitUntilToPossibleSetRenderTarget(m_luminanceAvgInTonemapLastFrameRt);
		// �����_�����O�^�[�Q�b�g��ݒ�
		rc.SetRenderTargetAndViewport(m_luminanceAvgInTonemapLastFrameRt);
		m_copyLuminanceAvgInTonemapSprite.Draw(rc);
		// �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
		rc.WaitUntilFinishDrawingToRenderTarget(m_luminanceAvgInTonemapLastFrameRt);

		g_graphicsEngine->EndGPUEvent();
	}
}