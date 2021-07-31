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
	static const int NUM_RENDER_TARGETS_DRAW_ONE_FRAME = 2;		//1�t���[���ɕ`�悷��A���ϋP�x�����߂邽�߂̃����_�[�^�[�Q�b�g�̍ő吔�B
private:
	/// <summary>
	/// ���ϋP�x�����߂邽�߂̕`��B
	/// </summary>
	/// <param name="rc">�����_�[�R���e�L�X�g�B</param>
	/// <param name="numberRenderTarget">�����_�[�^�[�Q�b�g�̔ԍ��B</param>
	void RenderToLuma(RenderContext& rc,int numberRenderTarget)
	{
		//�`��\��̃����_�[�^�[�Q�b�g��PRESENT����RENDERTARGET�ցB
		rc.WaitUntilToPossibleSetRenderTarget(*m_renderTargetVector[numberRenderTarget].get());
		//�����_�����O�^�[�Q�b�g��ݒ�
		rc.SetRenderTargetAndViewport(*m_renderTargetVector[numberRenderTarget].get());
		//�`��B
		m_spriteVector[numberRenderTarget].get()->Draw(rc);
		//���̃����_�[�^�[�Q�b�g�`��̍ۂɁB
		//�V�F�[�_�[���\�[�X�r���[�Ƃ��Ďg�p����̂ŁARENDERTARGET����PRESENT�ցB
		rc.WaitUntilFinishDrawingToRenderTarget(*m_renderTargetVector[numberRenderTarget].get());
	}
	struct TonemapBuffer {
		float scaler = 7.0f;
		float gammaCorrection = 2.0f;
	};

	std::vector<std::unique_ptr<RenderTarget>> m_renderTargetVector;		//���ϋP�x�����߂邽�߂̃����_�\�^�[�Q�b�g�B
	std::vector<std::unique_ptr<Sprite>> m_spriteVector;
	Sprite m_finalSprite;
	TonemapBuffer m_tonemapBuffer;
	int m_numberCalcRenderTarget = -1;		
}; 

