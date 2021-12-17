#pragma once

class Player : public IGameObject
{
public:
	enum EnPlayerState
	{
		enPlayerState_Idle,					//待機中。
		enPlayerState_Run,					//走る。
		enPlayerState_Jump,					//ジャンプ中。
		enPlayerState_GameOver,				//ゲームオーバー。
		enPlayerState_GameClear,			//ゲームクリアー。
		enPlayerState_GameClear_Idle,		//ゲームクリア(待機中)。
		enPlayerState_Num
	};
public:
	////////////////////////////////////
	// メンバ関数
	////////////////////////////////////
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
	/// 初期座標を設定。
	/// </summary>
	/// <param name="startPosition">初期座標。</param>
	void SetStartPosition(const Vector3& startPosition)
	{
		m_startPosition = startPosition;
	}
	/// <summary>
	/// プレイヤーのステート(状態)を取得。
	/// </summary>
	/// <returns>プレイヤーの状態。</returns>
	const EnPlayerState GetPlayerState() const
	{
		return m_playerState;
	}
	const Vector3& GetForward() const
	{
		return m_forward;
	}
	void NotifyGameOver();
	void NotifyGameClear();
	void ReStart();
private:
	/// <summary>
	/// 移動処理。
	/// </summary>
	void Move();
	/// <summary>
	/// 回転処理。
	/// </summary>
	void Turn();
	/// <summary>
	/// ステートを管理。
	/// </summary>
	void ManageState();
	/// <summary>
	/// アニメーションの再生。
	/// </summary>
	void PlayAnimation();
	ModelRender			m_modelRender;				//モデルレンダ―。
	Vector3				m_position;					//座標。
	Vector3				m_startPosition;			//初期座標。
	CharacterController m_charaCon;					//キャラクターコントローラー。
	Vector3				m_moveSpeed;				//移動速度。
	Quaternion			m_rotation;					//クォータニオン。
	enum EnAnimationClip {							//アニメーション。
		enAnimationClip_Idle,
		enAnimationClip_Run,
		enAnimationClip_Jump,
		enAnimationClip_GameClear,
		enAnimationClip_GameOver,
		enAnimationClip_Num,
	};
	AnimationClip		m_animationClips[enAnimationClip_Num];		//アニメーションクリップ。
	EnPlayerState		m_playerState = enPlayerState_Idle;				//プレイヤーのステート(状態)を表す変数。
	Vector3				m_forward;
	std::vector<PointLight* > m_pointLightList;	// ポイントライト。
	SpotLight m_spotLight;						// スポットライト。
	VolumeSpotLight m_volumeSpotLight;			// ボリュームスポットライト。
};

