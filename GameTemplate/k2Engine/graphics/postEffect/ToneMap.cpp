#include "k2EnginePreCompile.h"
#include "ToneMap.h"

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
	ToneMap::ToneMap()
	{

	}

	ToneMap::~ToneMap()
	{

	}

	void ToneMap::OnInit(
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
		for (auto& rt : m_avgRt) {
			rt.Create(
				1,
				1,
				1,
				1,
				DXGI_FORMAT_R16_FLOAT,
				DXGI_FORMAT_UNKNOWN
			);
		}

		m_finalRt.Create(
			mainRenderTarget.GetWidth(),
			mainRenderTarget.GetHeight(),
			1,
			1,
			mainRenderTarget.GetColorBufferFormat(),
			DXGI_FORMAT_UNKNOWN
		);
		m_tonemapParam.midddleGray = 0.4f;
		m_tonemapParam.deltaTime = 1.0f / 60.0f;
		// �ΐ����ς��Ƃ�X�v���C�g���������B
		int curRtNo = 0;
		{
			SpriteInitData initData;
			initData.m_width = m_calcAvgRt[curRtNo].GetWidth();
			initData.m_height = m_calcAvgRt[curRtNo].GetHeight();
			initData.m_colorBufferFormat[0] = m_calcAvgRt[curRtNo].GetColorBufferFormat();
			initData.m_fxFilePath = "Assets/shader/tonemap.fx";
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
			initData.m_fxFilePath = "Assets/shader/tonemap.fx";
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
			initData.m_fxFilePath = "Assets/shader/tonemap.fx";
			initData.m_psEntryPoinFunc = "PSCalcLuminanceExpAvarage";
			initData.m_expandConstantBuffer = m_avSampleOffsets;
			initData.m_expandConstantBufferSize = sizeof(m_avSampleOffsets);
			initData.m_textures[0] = &m_calcAvgRt[curRtNo - 1].GetRenderTargetTexture();
			m_calcAvgSprites[curRtNo].Init(initData);
		}
		// ���Ï���
		{
			SpriteInitData initData;
			initData.m_width = mainRenderTarget.GetWidth();
			initData.m_height = mainRenderTarget.GetHeight();
			initData.m_colorBufferFormat[0] = m_calcAvgRt[enCalcAvgExp].GetColorBufferFormat();
			initData.m_fxFilePath = "Assets/shader/tonemap.fx";
			initData.m_psEntryPoinFunc = "PSCalcAdaptedLuminance";
			initData.m_expandConstantBuffer = &m_tonemapParam;
			initData.m_expandConstantBufferSize = sizeof(m_tonemapParam);
			initData.m_textures[0] = &m_calcAvgRt[enCalcAvgExp].GetRenderTargetTexture();
			initData.m_textures[1] = &m_avgRt[0].GetRenderTargetTexture();
			initData.m_textures[2] = &m_avgRt[1].GetRenderTargetTexture();

			m_calcAdapteredLuminanceSprite.Init(initData);
		}
		{
			SpriteInitData initData;
			initData.m_width = mainRenderTarget.GetWidth();
			initData.m_height = mainRenderTarget.GetHeight();
			initData.m_colorBufferFormat[0] = m_calcAvgRt[enCalcAvgExp].GetColorBufferFormat();
			initData.m_fxFilePath = "Assets/shader/tonemap.fx";
			initData.m_psEntryPoinFunc = "PSCalcAdaptedLuminanceFirst";
			initData.m_expandConstantBuffer = &m_tonemapParam;
			initData.m_expandConstantBufferSize = sizeof(m_tonemapParam);
			initData.m_textures[0] = &m_calcAvgRt[enCalcAvgExp].GetRenderTargetTexture();

			m_calcAdapteredLuminanceFisrtSprite.Init(initData);
		}
		// �ŏI����
		{
			SpriteInitData initData;
			initData.m_width = mainRenderTarget.GetWidth();
			initData.m_height = mainRenderTarget.GetHeight();
			initData.m_colorBufferFormat[0] = mainRenderTarget.GetColorBufferFormat();
			initData.m_fxFilePath = "Assets/shader/tonemap.fx";
			initData.m_psEntryPoinFunc = "PSFinal";
			initData.m_expandConstantBuffer = &m_tonemapParam;
			initData.m_expandConstantBufferSize = sizeof(m_tonemapParam);
			initData.m_textures[0] = &mainRenderTarget.GetRenderTargetTexture();
			initData.m_textures[1] = &m_avgRt[0].GetRenderTargetTexture();
			initData.m_textures[2] = &m_avgRt[1].GetRenderTargetTexture();
			m_finalSprite.Init(initData);
		}
	}
	void ToneMap::CalcLuminanceAvarage(RenderContext& rc)
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
	void ToneMap::OnRender(RenderContext& rc, RenderTarget& mainRenderTarget)
	{
		if (!m_isEnable) {
			return;
		}
		CalcLuminanceAvarage(rc);

		// ���Ï���
		m_tonemapParam.currentAvgTexNo = m_currentAvgRt;
		m_tonemapParam.deltaTime = g_gameTime->GetFrameDeltaTime();
		rc.WaitUntilToPossibleSetRenderTarget(m_avgRt[m_currentAvgRt]);
		// �����_�����O�^�[�Q�b�g��ݒ�
		rc.SetRenderTargetAndViewport(m_avgRt[m_currentAvgRt]);

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
		rc.WaitUntilFinishDrawingToRenderTarget(m_avgRt[m_currentAvgRt]);


		// �ŏI�����B
		// �����_�����O�^�[�Q�b�g�Ƃ��ė��p�ł���܂ő҂�
		rc.WaitUntilToPossibleSetRenderTarget(m_finalRt);
		// �����_�����O�^�[�Q�b�g��ݒ�
		rc.SetRenderTargetAndViewport(m_finalRt);
		// �ŏI�����B
		m_finalSprite.Draw(rc);

		// �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
		rc.WaitUntilFinishDrawingToRenderTarget(m_finalRt);

		m_currentAvgRt = 1 ^ m_currentAvgRt;

	}
}