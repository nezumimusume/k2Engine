#pragma once

class Background : public IGameObject
{
public:
	//////////////////////////////////////
	// メンバ関数。
	//////////////////////////////////////
	Background();
	~Background();
	bool Start();
	
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
	/// 大きさを設定。
	/// </summary>
	/// <param name="scale">大きさ。</param>
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}
	/// <summary>
	/// 回転を設定。
	/// </summary>
	/// <param name="rotation">回転。</param>
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
private:
	ModelRender			m_modelRender;							//モデルレンダラー。
	PhysicsStaticObject m_phyStaticObject;						//静的物理オブジェクト。
	Vector3				m_position;								//座標。
	Quaternion			m_rotation = Quaternion::Identity;		//回転。
	Vector3				m_scale = Vector3::One;					//大きさ。
};