#pragma once


/// <summary>
/// レバー。
/// </summary>
class Lever : public IGameObject
{
public:
	/// <summary>
	/// レバーステート。
	/// </summary>
	enum EnLeverState {
		enLeverState_Idle,				//最初の待機状態。
		enLeverState_Push,				//押す。
		enLeverState_Push_Idle,			//押し終わった。
		enLeverState_Pull,				//引く。
		enLeverState_Pull_Idle			//引き終わった。
	};
public:
	Lever();
	~Lever();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	/// <summary>
	/// 座標を設定する。
	/// </summary>
	/// <param name="position">座標。</param>
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	/// <summary>
	/// 大きさを設定する。
	/// </summary>
	/// <param name="scale">大きさ。</param>
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}
	/// <summary>
	/// 回転を設定する。
	/// </summary>
	/// <param name="rotation">回転。</param>
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
	/// <summary>
	/// レバーの番号を設定する。
	/// 設定した番号と同じドアを動かします。
	/// </summary>
	/// <param name="leverNumber">番号。</param>
	void SetLeverNumber(const int leverNumber)
	{
		m_leverNumber = leverNumber;
	}
	/// <summary>
	/// レバーの番号を取得する。
	/// </summary>
	/// <returns>番号。</returns>
	const int GetLeverNumber() const
	{
		return m_leverNumber;
	}
private:
	/// <summary>
	/// 押すステートに移行する処理。
	/// </summary>
	void ProcessTransitionPushState();
	/// <summary>
	/// 押し終わったステートに移行する処理。
	/// </summary>
	void ProcessTransitionPushIdleState();
	/// <summary>
	/// 引くステートに移行する処理。
	/// </summary>
	void ProcessTransitionPullState();
	/// <summary>
	/// 引き終わったステートに行こうする処理。
	/// </summary>
	void ProcessTransitionPullIdleState();
	/// <summary>
	/// アニメーションを再生する処理。
	/// </summary>
	void PlayAnimation();
	/// <summary>
	/// ステート管理。
	/// </summary>
	void ManageState();
	/// <summary>
	/// 待機状態のステート遷移処理。
	/// </summary>
	void ProcessIdleStateTransition();
	/// <summary>
	/// 押すステートの遷移処理。
	/// </summary>
	void ProcessPushStateTransition();
	/// <summary>
	/// 押し終わったステートの遷移処理。
	/// </summary>
	void ProcessPushIdleStateTransition();
	/// <summary>
	/// 引くステートの遷移処理。
	/// </summary>
	void ProcessPullStateTransition();
	/// <summary>
	/// 引き終わったステートの遷移処理。
	/// </summary>
	void ProcessPullIdleStateTransition();
	
	enum EnAnimationClip {					//アニメーション。
		enAnimationClip_Idle,				//待機。
		enAnimationClip_Push,				//押す。
		enAnimationClip_Pull,				//引く。
		enAnimationClip_Num,				//アニメーションの数。
	};
	AnimationClip			m_animationClips[enAnimationClip_Num];		//アニメーションクリップ。
	ModelRender				m_modelRender;								//モデルレンダー。
	Vector3					m_position;									//座標。
	Vector3					m_scale = Vector3::One;						//大きさ。
	Quaternion				m_rotation;									//回転。
	CollisionObject*		m_collisionObject = nullptr;				//コリジョンオブジェクト。
	EnLeverState			m_leverState = enLeverState_Idle;			//レバーステート。
	int						m_leverNumber = 0;							//レバーの番号。
};

