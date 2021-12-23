#pragma once
class Lantern : public IGameObject
{
public:
	Lantern();
	~Lantern();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}
private:
	ModelRender m_modelRender;
	Vector3 m_position;
	Quaternion m_rotation;
	Vector3 m_scale;
	SpotLight m_spotLight;
};

