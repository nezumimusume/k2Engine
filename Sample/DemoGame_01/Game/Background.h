#pragma once
class Background : public IGameObject
{
public:
	Background();
	~Background();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
private:
	ModelRender				m_modelRender;
	Vector3					m_position;
	Vector3					m_scale = Vector3::One;
	Quaternion				m_rotation;
	PhysicsStaticObject		m_physicsStaticObject;
};

