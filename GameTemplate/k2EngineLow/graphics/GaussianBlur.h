#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// �K�E�V�A���u���[�N���X�B
	/// </summary>
	class GaussianBlur : public Noncopyable {
	public:
		
		/// <summary>
		/// �������B
		/// </summary>
		/// <param name="originalTexture">�K�E�V�A���u���[��������I���W�i���e�N�X�`���B</param>
		void Init(Texture* originalTexture, bool isBlurAlpha = false, bool isDownSample = true);
		/// <summary>
		/// ������
		/// </summary>
		/// <param name="originalTexture"></param>
		/// <param name="isBlurAlpha">���Ƀu���[��������H</param>
		/// <param name="width">�u���[�̌��ʂ��o�͂���e�N�X�`���̕�</param>
		/// <param name="height">�u���[�̌��ʂ��o�͂���e�N�X�`���̍���</param>
		void Init(Texture* originalTexture, bool isBlurAlpha, int width, int height);
		/// <summary>
		/// �K�E�V�A���u���[��GPU��Ŏ��s�B
		/// </summary>
		/// <remarks>
		/// �{�֐��̌Ăяo���́ADirectX12�𗘗p�����`��R�}���h�������ɌĂяo���K�v������܂��B
		/// </remarks>
		/// <param name="rc">�����_�����O�^�[�Q�b�g</param>
		/// <param name="blurPower">�u���[�̋����B�l���傫���قǃ{�P�������Ȃ�B</param>
		void ExecuteOnGPU(RenderContext& rc, float blurPower);
		/// <summary>
		/// �{�P�e�N�X�`�����擾�B
		/// </summary>
		/// <returns></returns>
		Texture& GetBokeTexture()
		{
			return m_yBlurRenderTarget.GetRenderTargetTexture();
		}
	private:
		/// <summary>
		/// �����_�����O�^�[�Q�b�g���������B
		/// </summary>
		void InitRenderTargets();
		/// <summary>
		/// �X�v���C�g���������B
		/// </summary>
		void InitSprites(bool isBlurAlpha);
		/// <summary>
		/// �d�݃e�[�u�����X�V����B
		/// </summary>
		void UpdateWeightsTable(float blurPower);
	private:
		enum { NUM_WEIGHTS = 8 };				// �d�݂̐��B
		float m_weights[NUM_WEIGHTS];			// �d�݃e�[�u���B
		int m_bokeTextureWidth = 0;				// �{�P�e�N�X�`���̕��B
		int m_bokeTextureHeight = 0;			// �{�P�e�N�X�`���̍����B
		Texture* m_originalTexture = nullptr;	// �I���W�i���e�N�X�`���B
		RenderTarget m_xBlurRenderTarget;		// ���{�P�摜��`�悷�郌���_�����O�^�[�Q�b�g�B
		RenderTarget m_yBlurRenderTarget;		// �c�{�P�摜��`�悷�郌���_�����O�^�[�Q�b�g�B
		Sprite m_xBlurSprite;					// ���{�P�摜��`�悷�邽�߂̃X�v���C�g�B
		Sprite m_yBlurSprite;					// �c�{�P�摜��`�悷�邽�߂̃X�v���C�g�B
	};
}
