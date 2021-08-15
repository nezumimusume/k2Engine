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
	private:
		virtual void OnInit(
			RenderTarget& mainRenderTarget,
			RenderTarget& zprepassRenderTarget,
			RenderTarget& normalRenderTarget,
			RenderTarget& metallicSmoothRenderTarget,
			RenderTarget& albedoRenderTarget) = 0;
		virtual void OnRender(RenderContext& rc, RenderTarget& mainRenderTarget) = 0;
		virtual Texture& GetResultTexture() = 0;
		/// <summary>
		/// �|�X�g�̌��ʂ����C�������_�����O�^�[�Q�b�g�ɃR�s�[����H
		/// </summary>
		/// <returns></returns>
		virtual bool IsCopyResultTextureToMainRenderTarget() const
		{
			return false;
		}
	private:
		Sprite m_copyMainRtSprite;	// ���C�������_�����O�^�[�Q�b�g�ɕ`�悷�邽�߂̃X�v���C�g�B
	};
}