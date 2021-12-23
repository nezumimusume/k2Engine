#pragma once
class BackGround : public IGameObject
{
public:
	BackGround();
	~BackGround();
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
private:
	ModelRender					m_modelRender;
	PhysicsStaticObject			m_physicsStaticObject;
	Vector3 m_position;
	Vector3 m_scale = Vector3::One;
};

