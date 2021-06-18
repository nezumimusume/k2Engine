#pragma once

class Lever;

class Player : public IGameObject
{
public:
	Player();
	~Player();
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
	//動ける状態かどうかを取得。
	bool GetIsEnableMove() const
	{
		return m_playerState != 3 && 
			m_playerState != 4 && 
			m_playerState != 5 &&
			m_playerState != 6;
	}
private:
	void Move();
	void Rotation();
	void Attack();
	void MagicAttack();
	void PushLever();
	void ReceiveDamage();
	void MakeAttackCollision();
	void MakeFireBall();
	/// <summary>
	/// アニメーションの再生。
	/// </summary>
	void PlayAnimation();
	/// <summary>
	/// ステートを管理。
	/// </summary>
	void ManageState();
	//アニメーションイベント
	void OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName);
	enum EnAnimationClip {							//アニメーション。
		enAnimationClip_Idle,
		enAnimationClip_Walk,
		enAnimationClip_Run,
		enAnimationClip_Attack,
		enAnimationClip_MagicAttack,
		enAnimationClip_Damage,
		enAnimationClip_Down,
		enAnimationClip_PushLever,
		enAnimationClip_Num,
	};
	AnimationClip		m_animationClips[enAnimationClip_Num];		//アニメーションクリップ。
	ModelRender			m_modelRender;
	Vector3				m_position;
	Vector3				m_moveSpeed;				//移動速度。
	Vector3				m_forward = Vector3::AxisZ;
	Quaternion			m_rotation;					//クォータニオン。
	CharacterController	m_charaCon;
	int					m_playerState = 0;
	bool				m_isAttack = false;
	bool				m_isUnderAttack = false;
	bool				m_isMagicAttack = false;
	bool				m_isPushLever = false;
	bool				m_isReceiveDamage = false;
	int					m_swordBoneId = -1;
};

