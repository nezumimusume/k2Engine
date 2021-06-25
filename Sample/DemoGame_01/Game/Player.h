#pragma once

//クラス宣言。
class Lever;
class Collision;
class Game;

/// <summary>
/// プレイヤー。
/// </summary>
class Player : public IGameObject
{
public:
	//プレイヤーのステート。
	enum EnPlayerState {
		enPlayerState_Idle,					//待機。
		enPlayerState_Walk,					//歩き。
		enPlayerState_Run,					//走る。
		enPlayerState_Attack,				//攻撃。
		enPlayerState_MagicAttack,			//魔法攻撃。
		enPlayerState_PushLever,			//レバーを押す。
		enPlayerState_ReceiveDamage,		//ダメ―ジ受けた。
		enPlayerState_Down,					//HPが0。
		enPlayerState_Clear					//クリアー。
	};
public:
	Player();
	~Player();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	/// <summary>
	/// 座標を設定。
	/// </summary>
	/// <param name="position">座標。</param>
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	/// <summary>
	/// 座標を取得。
	/// </summary>
	/// <returns>座標。</returns>
	const Vector3& GetPosition() const
	{
		return m_position;
	}
	/// <summary>
	/// 回転を設定。
	/// </summary>
	/// <param name="rotation">回転。</param>
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
	/// <summary>
	/// 動ける状態(ステート)かどうかを取得。
	/// </summary>
	/// <returns>動ける状態(ステート)ならtrue。</returns>
	bool IsEnableMove() const
	{
		return m_playerState != enPlayerState_Attack &&
			m_playerState != enPlayerState_MagicAttack &&
			m_playerState != enPlayerState_ReceiveDamage &&
			m_playerState != enPlayerState_PushLever &&
			m_playerState != enPlayerState_Down &&
			m_playerState != enPlayerState_Clear;
	}
private:
	/// <summary>
	/// 移動処理。
	/// </summary>
	void Move();
	/// <summary>
	/// 回転処理。
	/// </summary>
	void Rotation();
	/// <summary>
	/// 攻撃中の処理。
	/// </summary>
	void Attack();
	/// <summary>
	/// 敵の攻撃との当たり判定処理。
	/// </summary>
	void Collision();
	/// <summary>
	/// 攻撃の際の当たり判定用のコリジョンを作成する。
	/// </summary>
	void MakeAttackCollision();
	/// <summary>
	/// 魔法攻撃のファイヤーボールを作成する。
	/// </summary>
	void MakeFireBall();
	/// <summary>
	/// レバーを押す時の、当たり判定用のコリジョンを作成する。
	/// </summary>
	void MakePushLeverCollision();
	/// <summary>
	/// 攻撃した際のエフェクトを作成する。
	/// </summary>
	void MakeSlashingEffect();
	/// <summary>
	/// アニメーションの再生。
	/// </summary>
	void PlayAnimation();
	/// <summary>
	/// 各ステートの遷移処理。
	/// </summary>
	void ManageState();
	/// <summary>
	/// アニメーションイベント用の関数。
	/// </summary>
	/// <param name="clipName">アニメーションの名前。</param>
	/// <param name="eventName">アニメーションイベントのキーの名前。</param>
	void OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName);
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
	/// 攻撃ステートの遷移処理。
	/// </summary>
	void ProcessAttackStateTransition();
	/// <summary>
	/// 魔法攻撃ステートの遷移処理。
	/// </summary>
	void ProcessMagicAttackStateTransition();
	/// <summary>
	/// レバーを押すステートの遷移処理。
	/// </summary>
	void ProcessPushLeverStateTransition();
	/// <summary>
	/// 被ダメージステートの遷移処理。
	/// </summary>
	void ProcessReceiveDamageStateTransition();
	/// <summary>
	/// ダウンステートの遷移処理。
	/// </summary>
	void ProcessDownStateTransition();
	/// <summary>
	/// クリアステートの遷移処理。
	/// </summary>
	void ProcessClearStateTransition();
	
	//アニメーション。
	enum EnAnimationClip {
		enAnimationClip_Idle,				//待機アニメーション。	
		enAnimationClip_Walk,				//歩きアニメーション。
		enAnimationClip_Run,				//走りアニメーション。
		enAnimationClip_Attack,				//攻撃アニメーション。
		enAnimationClip_MagicAttack,		//魔法攻撃アニメーション。
		enAnimationClip_Damage,				//被ダメージアニメーション。
		enAnimationClip_Down,				//ダウンアニメーション。
		enAnimationClip_PushLever,			//レバーを押すアニメーション。
		enAnimationClip_Winner,				//勝利アニメーション。
		enAnimationClip_Num,				//アニメーションの数。
	};
	AnimationClip			m_animationClips[enAnimationClip_Num];		//アニメーションクリップ。
	ModelRender				m_modelRender;								//モデルレンダ―。
	Vector3					m_position;									//座標。
	Vector3					m_moveSpeed;								//移動速度。
	Vector3					m_forward = Vector3::AxisZ;					//プレイヤーの正面ベクトル。
	Quaternion				m_rotation;									//クォータニオン。
	CharacterController		m_charaCon;									//キャラクターコントローラー。
	EnPlayerState			m_playerState = enPlayerState_Idle;			//プレイヤーステート。
	bool					m_isUnderAttack = false;					//攻撃中ならtrue。
	int						m_swordBoneId = -1;							//剣に設定したボーンのID。
	int						m_hp = 10;									//HP。
	Game*					m_game = nullptr;							//ゲーム。
};

