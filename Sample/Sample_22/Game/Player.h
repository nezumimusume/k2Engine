

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
	////////////////////////////////////
	// メンバ変数とか
	////////////////////////////////////
	ModelRender m_modelRender;
	Vector3 m_position;						//座標。
	Vector3 m_scale = Vector3::One;			//大きさ。
	CharacterController m_charaCon;			//キャラクターコントローラー。
	Vector3 m_moveSpeed;					//移動速度。
	Quaternion m_rotation;					//クォータニオン。
};

