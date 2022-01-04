#pragma once

#include "graphics/postEffect/PostEffectComponentBase.h"

namespace nsK2Engine {
    /// <summary>
    /// �V�[���̕��ϋP�x�̌v�Z�����B
    /// </summary>
    class CalcSceneLuminance : public PostEffectComponentBase
	{
	public:
		/// <summary>
		/// �`��B
		/// </summary>
		/// <param name="rc">�����_�\�R���e�L�X�g�B</param>
		/// <param name="mainRenderTarget">���C�������_�\�^�[�Q�b�g�B</param>
		void OnRender(RenderContext& rc, RenderTarget& mainRenderTarget) override;
		/// <summary>
		/// �������B
		/// </summary>
		void OnInit(
			RenderTarget& mainRenderTarget,
			RenderTarget& zprepassRenderTarget,
			RenderTarget& normalRenderTarget,
			RenderTarget& metallicSmoothRenderTarget,
			RenderTarget& albedoRenderTarget) override;
		/// <summary>
		/// �|�X�g�̌��ʂ̉摜�����C�������_�����O�^�[�Q�b�g�ɃR�s�[����H
		/// </summary>
		/// <returns></returns>
		bool IsCopyResultTextureToMainRenderTarget() const override
		{
			return false;
		}
		/// <summary>
		/// �V�[�����؂�ւ�������Ƃ�ʒm�B
		/// </summary>
		/// <param name="timer"></param>
		void NotifyChangeScene(float timer)
		{
			m_isFirstWhenChangeScene = timer;
			m_changeSceneTimer = timer;
		}
		/// <summary>
		/// �V�[���̕��ϋP�x���L������Ă���e�N�X�`�����擾�B
		/// </summary>
		/// <returns></returns>
		Texture& GetLuminanceAvgTextureInScene()
		{
			return m_luminanceAvgInTonemapRt.GetRenderTargetTexture();
		}
		
	private:
		/// <summary>
		/// ���ϋP�x���v�Z����B
		/// </summary>
		/// <param name="">�����_�����O�R���e�L�X�g</param>
		void CalcLuminanceAvarage(RenderContext& rc);
	private:
		static const int MAX_SAMPLES = 16;

		// �V�F�[�_�[����b0���W�X�^�Ɋ֘A�t�������f�[�^�B
		struct SCB_1 {
			float deltaTime;
		};
		enum CalcAvgSprite {
			enCalcAvgLog,					// �ΐ����ς����߂�B
			enCalcAvg_Start,
			enCalcAvg_0 = enCalcAvg_Start,	// ���ϋP�x���v�Z�B
			enCalcAvg_1,					// ���ϋP�x���v�Z�B
			enCalcAvg_2,					// ���ϋP�x���v�Z�B	
			enCalcAvg_3,					// ���ϋP�x���v�Z����B
			enCalcAvg_End,
			enCalcAvgExp = enCalcAvg_End,	// exp()��p���čŏI���ς����߂�B
			enNumCalcAvgSprite
		};
		RenderTarget m_calcAvgRt[enNumCalcAvgSprite];		// ���ϋP�x�v�Z�p�̃����_�����O�^�[�Q�b�g�B
		RenderTarget m_luminanceAvgInTonemapLastFrameRt;	// 1�t���[���O�̃g�[���}�b�v�Ŏg�p�������ϋP�x���L�����邽�߂̃����_�����O�^�[�Q�b�g�B
		RenderTarget m_luminanceAvgInTonemapRt;				// ���݂̃t���[���̃g�[���}�b�v�Ŏg�p���镽�ϋP�x���L�����邽�߂̃����_�����O�^�[�Q�b�g�B			
		Sprite m_calcAvgSprites[enNumCalcAvgSprite];
		Sprite m_calcAdapteredLuminanceSprite;			// ���Ï����p�̃X�v���C�g�B
		Sprite m_calcAdapteredLuminanceFisrtSprite;		// ���Ï����p�̃X�v���C�g�B(�V�[�����؂�ւ�����Ƃ��Ɏg�p�����B)
		Sprite m_copyLuminanceAvgInTonemapSprite;		// �g�[���}�b�v�Ŏg�p�������ϋP�x�R�s�[�p�̃X�v���C�g�B
		bool m_isFirstWhenChangeScene = true;			// �V�[�����؂�ւ���ď���̕`�悩�ǂ����̃t���O�B
		Vector4 m_avSampleOffsets[MAX_SAMPLES];
		SCB_1 m_cb1;
		float m_changeSceneTimer = 0.5f;				// �V�[���؂�ւ��^�C�}�[�B
	};
}