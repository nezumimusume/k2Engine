#pragma once

class CollisionObject;

class Lever : public IGameObject
{
public:
	enum EnLeverState {
		enLeverState_Idle,
		enLeverState_Push,
		enLeverState_Push_Idle,
		enLeverState_Pull,
		enLeverState_Pull_Idle
	};
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
private:
	void ProcessTransitionPushState();
	void ProcessTransitionPushIdleState();
	void ProcessTransitionPullState();
	void ProcessTransitionPullIdleState();
	void PlayAnimation();
	void ManageState();
	void ProcessIdleStateTransition();
	void ProcessPushStateTransition();
	void ProcessPushIdleStateTransition();
	void ProcessPullStateTransition();
	void ProcessPullIdleStateTransition();


	
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
	EnLeverState			m_leverState = enLeverState_Idle;
	int						m_leverNumber = 0;
};

