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
		/// �V�[���̖��邳�̊�l��ݒ肷��B
		/// </summary>
		/// <param name="middlegray"></param>
		void SetMiddlegray(float middlegray)
		{
			m_cb1.midddleGray = middlegray;
		}
		/// <summary>
		/// �V�[���̖��邳�̊�l���擾����B
		/// </summary>
		/// <param name="middlegray"></param>
		float GetMiddlegray() const
		{
			return m_cb1.midddleGray;
		}
		
	private:
		static const int MAX_SAMPLES = 16;
		struct SCB_1 {
			float midddleGray;
		};
		Sprite m_finalSprite;							// �ŏI�����p�̃X�v���C�g�B
		RenderTarget m_finalRt;							// �ŏI���������_�����O�^�[�Q�b�g�B
		SCB_1 m_cb1;
	};
}