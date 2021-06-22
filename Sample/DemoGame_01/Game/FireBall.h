#pragma once

class EffectEmitter;
class CollisionObject;

class FireBall : public IGameObject
{
public:
	enum EnMagician {
		enMagician_None,
		enMagician_Player,
		enMagician_Enemy
	};
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
	/// <summary>
	/// èpé“Çê›íËÇ∑ÇÈÅB
	/// </summary>
	/// <param name="enMagician">èpé“ÅB</param>
	void SetEnMagician(const EnMagician enMagician)
	{
		m_enMagician = enMagician;
	}
private:
	Vector3					m_position;
	Quaternion				m_rotation;
	Vector3					m_moveSpeed;
	EffectEmitter*			m_effectEmitter = nullptr;
	float					m_timer = 0.0f;
	CollisionObject*		m_collisionObject;
	EnMagician				m_enMagician = enMagician_None;
};

