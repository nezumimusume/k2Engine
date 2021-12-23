

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
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
	const Vector3& GetPosition() const
	{
		return m_position;
	}
private:
	void Move();
	void Rotation();
	////////////////////////////////////
	// メンバ変数とか
	////////////////////////////////////
	ModelRender					m_modelRender;
	Vector3						m_position;						//座標。
	CharacterController			m_charaCon;						//キャラクターコントローラー。
	Vector3						m_moveSpeed;					//移動速度。
	Quaternion					m_rotation;						//回転。
	SpotLight					m_spotLight;
	Vector3						m_forward = Vector3::AxisZ;
};

