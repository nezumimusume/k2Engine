#pragma once

class EffectEmitter;
class CollisionObject;

class FireBall : public IGameObject
{
public:
	FireBall();
	~FireBall();
	bool Start();
	void Update();
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}

private:
	Vector3				m_position;
	Quaternion			m_rotation;
	Vector3				m_moveSpeed;
	EffectEmitter*		m_effectEmitter = nullptr;
	float				m_timer = 0.0f;
	CollisionObject*	m_collisionObject;
};

