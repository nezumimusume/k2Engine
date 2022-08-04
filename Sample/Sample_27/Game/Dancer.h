#pragma once
class Dancer : public IGameObject
{
public:
	Dancer();
	~Dancer();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	const Vector3& GetPosition() {
		return m_position;
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
	ModelRender			m_modelRender;
	AnimationClip		m_animationClip;
	Vector3				m_position;
	Quaternion			m_rotation;
	Vector3				m_scale = Vector3::One;
};

