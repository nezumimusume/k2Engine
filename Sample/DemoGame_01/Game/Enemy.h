#pragma once

class Player;

class Enemy : public IGameObject
{
public:
	enum EnEnemyState {
		enEnemyState_Idle,
		enEnemyState_Chase,
		enEnemyState_Attack,
		enEnemyState_MagicAttack,
		enEnemyState_ReceiveDamage,
		enEnemyState_Down,
	};
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
	void SetHP(const int hp)
	{
		m_hp = hp;
	}
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}
	//動ける状態かどうかを取得。
	const bool GetIsEnableMove() const
	{
		return m_enemyState != enEnemyState_Attack &&
			m_enemyState != enEnemyState_ReceiveDamage &&
			m_enemyState != enEnemyState_Down;
	}
private:
	void Chase();
	void Rotation();
	void Attack();
	void ReceiveDamage();
	void Down();
	void Collision();
	const bool SearchPlayer() const;
	void MakeAttackCollision();
	void MakeFireBall();
	/// <summary>
	/// アニメーションの再生。
	/// </summary>
	void PlayAnimation();
	/// <summary>
	/// 各ステートの遷移処理。
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
		enAnimationClip_Num,
	};
	/// <summary>
	/// 共通のステート遷移処理。
	/// </summary>
	void ProcessCommonStateTransition();
	/// <summary>
	/// 待機ステートの遷移処理。
	/// </summary>
	void ProcessIdleStateTransition();
	/// <summary>
	/// 歩きステートの遷移処理。
	/// </summary>
	void ProcessWalkStateTransition();
	/// <summary>
	/// 走りステートの遷移処理。
	/// </summary>
	void ProcessRunStateTransition();
	/// <summary>
	/// 追跡ステートの背遷移処理。
	/// </summary>
	void ProcessChaseStateTransition();
	/// <summary>
	/// 攻撃ステートの遷移処理。
	/// </summary>
	void ProcessAttackStateTransition();
	/// <summary>
	/// 魔法攻撃ステートの遷移処理。
	/// </summary>
	void ProcessMagicAttackStateTransition();
	/// <summary>
	/// 被ダメージステートの遷移処理。
	/// </summary>
	void ProcessReceiveDamageStateTransition();
	/// <summary>
	/// ダウンステートの遷移処理。
	/// </summary>
	void ProcessDownStateTransition();
	/// <summary>
	/// 攻撃できる距離かどうか調べる。
	/// </summary>
	/// <returns>攻撃できるならtrue。</returns>
	const bool IsCanAttack() const;
	
	AnimationClip			m_animationClips[enAnimationClip_Num];		//アニメーションクリップ。
	ModelRender				m_modelRender;
	Vector3					m_position;
	Vector3					m_spawnPosition;
	Vector3					m_moveSpeed;				//移動速度。
	Vector3					m_forward = Vector3::AxisZ;
	Quaternion				m_rotation;					//クォータニオン。
	Vector3					m_scale = Vector3::One;
	CharacterController		m_charaCon;

	EnEnemyState			m_enemyState = enEnemyState_Idle;
	bool					m_isUnderAttack = false;
	int						m_swordBoneId = -1;
	int						m_hp = 10;
	Player*					m_player = nullptr;
	float					m_chaseTimer = 0.0f;
	float					m_idleTimer = 0.0f;
};


