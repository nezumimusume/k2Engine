#pragma once
class ToneMap : public Noncopyable
{
public:
	ToneMap();
	~ToneMap();
	/// <summary>
	/// �`��B
	/// </summary>
	/// <param name="rc">�����_�\�R���e�L�X�g�B</param>
	/// <param name="mainRenderTarget">���C�������_�\�^�[�Q�b�g�B</param>
	void Render(RenderContext& rc, RenderTarget& mainRenderTarget);
	/// <summary>
	/// �������B
	/// </summary>
	void Init(RenderTarget& mainRenderTarget);
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
			enCalcAvg_3,
		enCalcAvg_End,
		enCalcAvgExp = enCalcAvg_End,		// exp()��p���čŏI���ς����߂�B
		enNumCalcAvgSprite
	};
	RenderTarget m_calcAvgRt[enNumCalcAvgSprite];	// ���ϋP�x�v�Z�p�̃����_�����O�^�[�Q�b�g�B
	RenderTarget m_avgRt[2];					// ���ϋP�x���i�[����郌���_�����O�^�[�Q�b�g�B
	int m_currentAvgRt = 0;						// 
	Sprite m_calcAvgSprites[enNumCalcAvgSprite];
	Sprite m_calcAdapteredLuminanceSprite;		// ���Ï����p�̃X�v���C�g�B
	Sprite m_calsAdapteredLuminanceFisrtSprite;	// ���Ï����p�̃X�v���C�g�B(�V�[�����؂�ւ�����Ƃ��Ɏg�p�����B)
	Sprite m_finalSprite;						// �ŏI�����p�̃X�v���C�g�B
	Sprite m_copySprite;

	bool m_isFirstWhenChangeScene = true;			//!<�V�[�����؂�ւ���ď���̕`�悩�ǂ����̃t���O�B
	Vector4 m_avSampleOffsets[MAX_SAMPLES];
	RenderTarget m_finalRt;						// �ŏI���������_�����O�^�[�Q�b�g�B
	STonemapParam m_tonemapParam;
}; 

