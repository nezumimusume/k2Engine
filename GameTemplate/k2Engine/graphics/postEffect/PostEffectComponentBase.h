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
		/// ポストエフェクトを無効にする。
		/// </summary>
		void Disable()
		{
			m_isEnable = false;
		}
		/// <summary>
		/// ポストエフェクトを有効にする。
		/// </summary>
		void Enable()
		{
			m_isEnable = true;
		}
		/// <summary>
		/// ポストエフェクトが有効か判定。
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
		/// ポストの結果をメインレンダリングターゲットにコピーする？
		/// </summary>
		/// <returns></returns>
		virtual bool IsCopyResultTextureToMainRenderTarget() const
		{
			return false;
		}
		
	private:
		bool m_isEnable = true;
		Sprite m_copyMainRtSprite;	// メインレンダリングターゲットに描画するためのスプライト。
	};
}