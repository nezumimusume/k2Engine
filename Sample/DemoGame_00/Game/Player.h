

class Player : public IGameObject
{
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
	CharacterController m_charaCon;					//キャラクターコントローラー。
	Vector3				m_moveSpeed;				//移動速度。
	Quaternion			m_rotation;					//クォータニオン。
	enum EnAnimationClip {							//アニメーション。
		enAnimationClip_Idle,
		enAnimationClip_Run,
		enAnimationClip_Jump,
		enAnimationClip_Num,
	};
	AnimationClip		m_animationClips[enAnimationClip_Num];		//アニメーションクリップ。
	int m_playerState = 0;							//プレイヤーのステート(状態)を表す変数。
};

