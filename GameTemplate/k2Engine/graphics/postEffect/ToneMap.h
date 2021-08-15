#pragma once

#include "graphics/postEffect/PostEffectComponentBase.h"

namespace nsK2Engine {
	class ToneMap : public PostEffectComponentBase
	{
	public:
		ToneMap();
		~ToneMap();
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
		/// �|�X�g�G�t�F�N�g�����s�������ʂƂȂ�e�N�X�`�����擾�B
		/// </summary>
		/// <returns></returns>
		Texture& GetResultTexture() override
		{
			return m_finalRt.GetRenderTargetTexture();
		}
		/// <summary>
		/// �|�X�g�̌��ʂ̉摜�����C�������_�����O�^�[�Q�b�g�ɃR�s�[����H
		/// </summary>
		/// <returns></returns>
		bool IsCopyResultTextureToMainRenderTarget() const override
		{
			return true;
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
		/// �g�[���}�b�v��L���ɂ���B
		/// </summary>
		void Enable()
		{
			m_isEnable = true;
		}
		/// <summary>
		/// �g�[���}�b�v�𖳌��ɂ���B
		/// </summary>
		void Disable()
		{
			m_isEnable = false;
		}
	private:
		/// <summary>
		/// ���ϋP�x���v�Z����B
		/// </summary>
		/// <param name="">�����_�����O�R���e�L�X�g</param>
		void CalcLuminanceAvarage(RenderContext& rc);
	private:
		static const int MAX_SAMPLES = 16;
		struct STonemapParam {
			float deltaTime;
			float midddleGray;
			int currentAvgTexNo;
		};
		enum CalcAvgSprite {
			enCalcAvgLog,						// �ΐ����ς����߂�B
			enCalcAvg_Start,
			enCalcAvg_0 = enCalcAvg_Start,	// ���ϋP�x���v�Z�B
			enCalcAvg_1,					// ���ϋP�x���v�Z�B
			enCalcAvg_2,					// ���ϋP�x���v�Z�B	
			enCalcAvg_3,					// ���ϋP�x���v�Z����B
			enCalcAvg_End,
			enCalcAvgExp = enCalcAvg_End,		// exp()��p���čŏI���ς����߂�B
			enNumCalcAvgSprite
		};
		RenderTarget m_calcAvgRt[enNumCalcAvgSprite];	// ���ϋP�x�v�Z�p�̃����_�����O�^�[�Q�b�g�B
		RenderTarget m_avgRt[2];					// ���ϋP�x���i�[����郌���_�����O�^�[�Q�b�g�B
		int m_currentAvgRt = 0;						// 
		Sprite m_calcAvgSprites[enNumCalcAvgSprite];
		Sprite m_calcAdapteredLuminanceSprite;		// ���Ï����p�̃X�v���C�g�B
		Sprite m_calcAdapteredLuminanceFisrtSprite;	// ���Ï����p�̃X�v���C�g�B(�V�[�����؂�ւ�����Ƃ��Ɏg�p�����B)
		Sprite m_finalSprite;						// �ŏI�����p�̃X�v���C�g�B

		bool m_isFirstWhenChangeScene = true;			//!<�V�[�����؂�ւ���ď���̕`�悩�ǂ����̃t���O�B
		Vector4 m_avSampleOffsets[MAX_SAMPLES];
		RenderTarget m_finalRt;						// �ŏI���������_�����O�^�[�Q�b�g�B
		STonemapParam m_tonemapParam;
		bool m_isEnable = true;					// �g�[���}�b�v���L���H
		float m_changeSceneTimer = 0.5f;		// �V�[���؂�ւ��^�C�}�[�B
	};
}