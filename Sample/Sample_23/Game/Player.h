

class Player : public IGameObject
{
public:
	
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
	
	ModelRender			m_modelRender;				//モデルレンダ―。
	Vector3				m_position;					//座標。
};

