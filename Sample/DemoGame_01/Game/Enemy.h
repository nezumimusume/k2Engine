#pragma once
class Enemy : public IGameObject
{
public:
	Enemy();
	~Enemy();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	const Vector3& GetPosition() const
	{
		return m_position;
	}
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
private:
	void Move();
	void Rotation();
	void Attack();
	/// <summary>
	/// アニメーションの再生。
	/// </summary>
	void PlayAnimation();
	/// <summary>
	/// ステートを管理。
	/// </summary>
	void ManageState();
	enum EnAnimationClip {							//アニメーション。
		enAnimationClip_Idle,
		enAnimationClip_Walk,
		enAnimationClip_Run,
		enAnimationClip_Attack,
		enAnimationClip_Damage,
		enAnimationClip_Down,
		enAnimationClip_Num,
	};
	AnimationClip		m_animationClips[enAnimationClip_Num];		//アニメーションクリップ。
	ModelRender			m_modelRender;
	Vector3				m_position;
	Vector3				m_moveSpeed;				//移動速度。
	Quaternion			m_rotation;					//クォータニオン。
	CharacterController	m_charaCon;
	int					m_EnemyState = 0;
	bool				m_isAttack = false;
};


