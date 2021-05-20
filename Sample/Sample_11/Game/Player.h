

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
	////////////////////////////////////
	// メンバ変数とか
	////////////////////////////////////
	ModelRender m_modelRender;
	Vector3 m_position;						//座標。
	CharacterController m_charaCon;			//キャラクターコントローラー。
	Vector3 m_moveSpeed;					//移動速度。
};

