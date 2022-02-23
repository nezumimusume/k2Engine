#pragma once

//クラス宣言。
class Player;
class StarRender;

class Star : public IGameObject
{
public:
	~Star();
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
	/// インスタンス番号を設定。
	/// </summary>
	/// <param name="instanceNo"></param>
	void SetInstanceNo(int instanceNo)
	{
		m_instanceNo = instanceNo;
	}
private:
	Vector3				m_position;						//座用。
	Vector3				m_scale = Vector3::One * 0.3f;	//大きさ。
	Player*				m_player = nullptr;
	bool				m_isGet = false;
	float				m_jumpSpeed = 0.0f;
	float				m_jumStartPosY = 0.0f;
	Quaternion			m_rotation;
	StarRender*			m_starRender = nullptr;
	int					m_instanceNo = 0;				// インスタンス番号。
};

