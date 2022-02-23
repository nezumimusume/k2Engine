#pragma once
//クラス宣言。
class Player;
class PyramidRender;

class Pyramid : public IGameObject
{
public:
	~Pyramid();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

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
	/// 拡大率を設定。
	/// </summary>
	/// <param name="scale"></param>
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}
	/// <summary>
	/// インスタンス番号を設定。
	/// </summary>
	/// <param name="instanceNo">インスタンス番号。</param>
	void SetInstanceNo(int instanceNo)
	{
		m_instanceNo = instanceNo;
	}
private:
	Vector3				m_position;						// 座標。
	Vector3				m_scale = Vector3::One * 0.3f;	// 大きさ。
	Player*				m_player = nullptr;
	bool				m_isGet = false;
	float				m_jumpSpeed = 0.0f;
	float				m_jumPyramidtPosY = 0.0f;
	Quaternion			m_rotation;
	PyramidRender*		m_pyramidRender = nullptr;
	int					m_instanceNo = 0;				// インスタンス番号。
};

