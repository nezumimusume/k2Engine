#pragma once
class MuscleRender;

class Muscle : public IGameObject
{
public:
	Muscle();
	~Muscle();
	bool Start();
	void Update();
	
	Vector3					m_position;					//座標。
	Vector3					m_scale;					//大きさ。
	Quaternion				m_rotation;					//回転。
	MuscleRender*			m_muscleRender = nullptr;	//スフィアレンダー。
};

