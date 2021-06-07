#pragma once
class Enemy : public IGameObject
{
public:
	////////////////////////////////////
	// メンバ関数
	////////////////////////////////////
	Enemy();
	~Enemy();
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
	/// 大きさを設定。
	/// </summary>
	/// <param name="scale">大きさ。</param>
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
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
	Vector3				m_scale = Vector3::One;		//スケール。
	Quaternion			m_rotation;					//クォータニオン。
	enum EnAnimationClip {							//アニメーション。
		enAnimationClip_Idle,
		enAnimationClip_Run,
		enAnimationClip_Num,
	};
	AnimationClip		m_animationClips[enAnimationClip_Num];		//アニメーションクリップ。
	int m_enemyState = 0;							//エネミーのステート(状態)を表す変数。
};

