#pragma once
class MuscleRender;
class Game;

class Muscle : public IGameObject
{
public:
	Muscle();
	~Muscle();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	
	Vector3					m_position;					// 座標。
	Vector3					m_scale;					// 大きさ。
	Quaternion				m_rotation;					// 回転。
	MuscleRender*			m_muscleRender = nullptr;	// スフィアレンダー。
	ModelRender				m_modelRender;
	Game*					m_game = nullptr;
	AnimationClip			m_animationClip;
	int						m_instanceNo = 0;			// インスタンス番号
};

