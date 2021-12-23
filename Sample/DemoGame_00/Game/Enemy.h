#pragma once

//クラス宣言。
class MovedPath;
class Player;
class Game;

class Enemy : public IGameObject
{
public:
	enum EnEnemyState
	{
		enEnemyState_Idle,
		enEnemyState_Run,
		enEnemyState_Num
	};
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
	/// パスデータを読み込む。
	/// </summary>
	/// <param name="number">番号。</param>
	void LoadPath(const int number);
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
	/// <summary>
	/// プレイヤーを探索する。
	/// </summary>
	void SearchPlayer();
	/// <summary>
	/// プレイヤーのポインタを取得する。
	/// </summary>
	/// <returns>プレイヤーのポインタを取得済みならtrue。</returns>
	bool GetPlayer()
	{
		if (m_player == nullptr)
		{
			m_player = FindGO<Player>("player");
			return false;
		}
		return true;
	}
	ModelRender			m_modelRender;				//モデルレンダ―。
	Vector3				m_position;					//座標。
	Vector3				m_moveVector;				//移動方向ベクトル。
	Vector3				m_scale = Vector3::One;		//スケール。
	Quaternion			m_rotation;					//クォータニオン。
	enum EnAnimationClip {							//アニメーション。
		enAnimationClip_Idle,
		enAnimationClip_Run,
		enAnimationClip_Num,
	};
	AnimationClip		m_animationClips[enAnimationClip_Num];		//アニメーションクリップ。
	EnEnemyState		m_enemyState = enEnemyState_Idle;			//エネミーのステート(状態)を表す変数。
	std::unique_ptr<MovedPath>	m_movedPath;
	Player*				m_player = nullptr;			//プレイヤー。
	SpotLight m_spotLight;
	VolumeSpotLight m_volumeSpotLight;
};

