#pragma once
class FontRender
{
public:
	/// <summary>
	/// 表示する文字を設定。
	/// </summary>
	/// <param name="text">表示する文字。</param>
	void SetText(const wchar_t* text)
	{
		m_text = std::unique_ptr<const wchar_t[]>(text);
	}
	/// <summary>
	/// 表示する文字を取得。
	/// </summary>
	/// <returns>表示する文字。</returns>
	const wchar_t* GetText() const
	{
		return m_text.get();
	}
	/// <summary>
	/// 座標を設定。
	/// </summary>
	/// <param name="position">座標。</param>
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	/// <summary>
	/// 座標を取得。
	/// </summary>
	/// <returns>座標。</returns>
	const Vector3& GetPosition() const
	{
		return m_position;
	}
	/// <summary>
	/// 大きさを設定。
	/// </summary>
	/// <param name="scale">大きさ。</param>
	void SetScale(const float scale)
	{
		m_scale = scale;
	}
	/// <summary>
	/// 大きさを取得。
	/// </summary>
	/// <returns>大きさ。</returns>
	const float GetScale() const
	{
		return m_scale;
	}
	/// <summary>
	/// 色を設定。
	/// </summary>
	/// <param name="color">色。</param>
	void SetColor(const Vector4& color)
	{
		m_color = color;
	}
	/// <summary>
	/// 色を取得。
	/// </summary>
	/// <returns>色。</returns>
	const Vector4& GetColor() const
	{
		return m_color;
	}
	/// <summary>
	/// 回転を設定。
	/// </summary>
	/// <param name="rotation">回転。</param>
	void SetRotation(const float rotation)
	{
		m_rotation = rotation;
	}
	/// <summary>
	/// 回転を取得。
	/// </summary>
	/// <returns>回転。</returns>
	const float GetRotation() const
	{
		return m_rotation;
	}
	/// <summary>
	/// ピボットを設定。
	/// </summary>
	/// <param name="pivot">ピボット。</param>
	void SetPivot(const Vector2& pivot)
	{
		m_pivot = pivot;
	}
	/// <summary>
	/// ピボットを取得。
	/// </summary>
	/// <returns>ピボット。</returns>
	const Vector2& GetPivot() const
	{
		return m_pivot;
	}
	/// <summary>
	/// 描画処理。
	/// </summary>
	/// <param name="rc">レンダ―コンテキスト。</param>
	void Draw(RenderContext& rc)
	{
		if (m_text == nullptr)
		{
			return;
		}
		m_font.Begin(rc);
		m_font.Draw(m_text.get(), Vector2(m_position.x, m_position.y), m_color, m_rotation, m_scale, m_pivot);
		m_font.End(rc);
	}
	/// <summary>
	/// 影のパラメータを設定。
	/// </summary>
	/// <param name="isDrawShadow">影を描画する？</param>
	/// <param name="shadowOffset">影を描画するときのピクセルのオフセット量。</param>
	/// <param name="shadowColor">影の色。</param>
	void SetShadowParam(bool isDrawShadow, float shadowOffset, const Vector4& shadowColor)
	{
		m_font.SetShadowParam(isDrawShadow, shadowOffset, shadowColor);
	}
private:
	Vector3								m_position = Vector3::Zero;			//座標。	
	float								m_scale = 1.0f;						//文字の大きさ。
	Vector4								m_color = g_vec4White;				//文字の色、デフォルトで白。
	float								m_rotation = 0.0f;					//回転。
	Vector2								m_pivot = Sprite::DEFAULT_PIVOT;	//ピボット。
	std::unique_ptr<const wchar_t[]>	m_text = nullptr;					//文字。
	Font								m_font;								//フォント。
};

