#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// フォント
	/// </summary>
	class Font : public Noncopyable {
	public:
		/// <summary>
		/// 描画開始
		/// </summary>
		/// <param name="rc">レンダリングコンテキスト。</param>
		void Begin(RenderContext& rc);
		
		/// <summary>
		/// 描画終了
		/// </summary>
		/// <param name="rc">レンダリングコンテキスト</param>
		void End(RenderContext& rc);
		/// <summary>
		/// 描画
		/// </summary>
		/// <param name="text"></param>
		/// <param name="position"></param>
		/// <param name="color"></param>
		/// <param name="rotation"></param>
		/// <param name="scale"></param>
		/// <param name="pivot"></param>
		void Draw(
			wchar_t const* text,
			const Vector2& position,
			const Vector4& color,
			float rotation,
			float scale,
			Vector2 pivot
		);
		/*!
			*@brief	影のパラメータを設定。
			*@param[in]	isDrawShadow		影を描く？
			*@param[in]	shadowOffset		影を描くときのピクセルのオフセット量。
			*@param[in]	shadowColor			影の色。
			*/
		
		/// <summary>
		/// 影のパラメータを設定
		/// </summary>
		/// <param name="isDrawShadow">影を描画する</param>
		/// <param name="shadowOffset">影オフセット</param>
		/// <param name="shadowColor">影カラー</param>
		void SetShadowParam(bool isDrawShadow, float shadowOffset, const Vector4& shadowColor)
		{
			m_isDrawShadow = isDrawShadow;
			m_shadowOffset = shadowOffset;
			m_shadowColor = shadowColor;
		}
	private:
		DirectX::SpriteBatch* m_spriteBatch = nullptr;		//　スプライトバッチ。
		DirectX::SpriteFont* m_spriteFont = nullptr;		//　スプライトフォント。
		bool m_isDrawShadow = false;						//　影を書く？
		float m_shadowOffset = 0.0f;						//　影を書くときのピクセルのオフセット量。
		Vector4 m_shadowColor;								//　影の色。
		Matrix m_scaleMat;
	};
}