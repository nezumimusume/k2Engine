#pragma once

class CollisionObject;

class Lever : public IGameObject
{
public:
	Lever();
	~Lever();
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
	void SetLeverNumber(const int leverNumber)
	{
		m_leverNumber = leverNumber;
	}
	const int GetLeverNumber() const
	{
		return m_leverNumber;
	}
	void PushLever();
	void PullLever();
private:
	void PlayAnimation();
	void OpenDoor();
	void CloseDoor();
	enum EnAnimationClip {							//アニメーション。
		enAnimationClip_Idle,
		enAnimationClip_On,
		enAnimationClip_Off,
		enAnimationClip_Num,
	};
	AnimationClip			m_animationClips[enAnimationClip_Num];		//アニメーションクリップ。
	ModelRender				m_modelRender;
	Vector3					m_position;
	Vector3					m_scale = Vector3::One;
	Quaternion				m_rotation;
	CollisionObject*		m_collisionObject = nullptr;
	int						m_leverState = 0;
	int						m_leverNumber = 0;
};

