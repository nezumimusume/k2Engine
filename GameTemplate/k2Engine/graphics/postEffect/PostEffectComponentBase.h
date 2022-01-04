#pragma once

namespace nsK2Engine {
	class PostEffectComponentBase : public Noncopyable {
	public:
		void Init(
			RenderTarget& mainRenderTarget,
			RenderTarget& zprepassRenderTarget,
			RenderTarget& normalRenderTarget,
			RenderTarget& metallicSmoothRenderTarget,
			RenderTarget& albedoRenderTarget
		);
		void Render(RenderContext& rc, RenderTarget& mainRenderTarget);
		/// <summary>
		/// �|�X�g�G�t�F�N�g�𖳌��ɂ���B
		/// </summary>
		void Disable()
		{
			m_isEnable = false;
		}
		/// <summary>
		/// �|�X�g�G�t�F�N�g��L���ɂ���B
		/// </summary>
		void Enable()
		{
			m_isEnable = true;
		}
		/// <summary>
		/// �|�X�g�G�t�F�N�g���L��������B
		/// </summary>
		/// <returns></returns>
		bool IsEnable() const
		{
			return m_isEnable;
		}
	private:
		virtual void OnInit(
			RenderTarget& mainRenderTarget,
			RenderTarget& zprepassRenderTarget,
			RenderTarget& normalRenderTarget,
			RenderTarget& metallicSmoothRenderTarget,
			RenderTarget& albedoRenderTarget) = 0;
		virtual void OnRender(RenderContext& rc, RenderTarget& mainRenderTarget) = 0;
		virtual Texture& GetResultTexture() 
		{
			static Texture nullTexture;
			return nullTexture;
		}
		/// <summary>
		/// �|�X�g�̌��ʂ����C�������_�����O�^�[�Q�b�g�ɃR�s�[����H
		/// </summary>
		/// <returns></returns>
		virtual bool IsCopyResultTextureToMainRenderTarget() const
		{
			return false;
		}
		
	private:
		bool m_isEnable = true;
		Sprite m_copyMainRtSprite;	// ���C�������_�����O�^�[�Q�b�g�ɕ`�悷�邽�߂̃X�v���C�g�B
	};
}