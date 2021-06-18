#pragma once
class Door : public IGameObject
{
public:
	Door();
	~Door();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	void Open();
	void Close();
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
	void SetDoorNumber(const int doorNumber)
	{
		m_doorNumber = doorNumber;
	}
	const int GetDoorNumber() const
	{
		return m_doorNumber;
	}
private:
	void PlayAnimation();
	void ReleasePhysicsObject();
	ModelRender					m_modelRender;
	Vector3						m_position;
	Quaternion					m_rotation;
	Vector3						m_scale = Vector3::One;
	enum EnAnimationClip {							//アニメーション。
		enAnimationClip_Idle,
		enAnimationClip_Open,
		enAnimationClip_Num,
	};
	AnimationClip				m_animationClips[enAnimationClip_Num];		//アニメーションクリップ。
	PhysicsStaticObject			m_physicsStaticObject;
	int							m_doorState = 0;
	int							m_doorNumber = 0;
};

