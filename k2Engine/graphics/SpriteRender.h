#pragma once

/// <summary>
/// スプライトレンラー。
/// </summary>
class SpriteRender
{
public:
	/// <summary>
	/// 初期化。
	/// 横幅と縦幅は、画像のプロパティなどで確認できるピクセル数を入れてください。
	/// </summary>
	/// <param name="filePath">ファイルパス。</param>
	/// <param name="w">画像の横幅。</param>
	/// <param name="h">画像の縦幅。</param>
	void Init(const char* filePath, const float w, const float h);
	/// <summary>
	/// 座標を設定。
	/// </summary>
	/// <param name="pos">座標。</param>
	void SetPosition(const Vector3& pos)
	{
		m_position = pos;
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
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}
	/// <summary>
	/// 大きさを取得。
	/// </summary>
	/// <returns>大きさ。</returns>
	const Vector3& GetScale() const
	{
		return m_scale;
	}
	/// <summary>
	/// 回転を設定。
	/// </summary>
	/// <param name="rot">回転。</param>
	void SetRotation(const Quaternion& rot)
	{
		m_rotation = rot;
	}
	/// <summary>
	/// 回転を取得。
	/// </summary>
	/// <returns>回転。</returns>
	const Quaternion& GetRotation() const
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
	/// 更新処理。
	/// </summary>
	void Update()
	{
		m_sprite.Update(m_position,
			m_rotation,
			m_scale,
			m_pivot);
	}
	/// <summary>
	/// 描画処理。
	/// </summary>
	/// <param name="rc">レンダーコンテキスト。</param>
	void Draw(RenderContext& rc)
	{
		m_sprite.Draw(rc);
	}
private:
	Sprite			m_sprite;								//スプライト。
	Vector3			m_position;								//座標。
	Quaternion		m_rotation = Quaternion::Identity;		//回転。
	Vector3			m_scale = Vector3::One;					//大きさ。
	Vector2			m_pivot = Sprite::DEFAULT_PIVOT;		//ピボット。

};

