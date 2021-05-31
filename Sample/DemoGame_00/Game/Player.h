

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
	void Move();
	void Turn();
	void ManageState();
	void PlayAnimation();
	////////////////////////////////////
	// メンバ変数とか
	////////////////////////////////////
	ModelRender			m_modelRender;
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

