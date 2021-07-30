#pragma once
class SphereRender;

class Sphere : public IGameObject
{
public:
	Sphere();
	~Sphere();
	bool Start();
	void Update();
	
	Vector3					m_position;					//座標。
	Vector3					m_scale;					//大きさ。
	Quaternion				m_rotation;					//回転。
	SphereRender*			m_sphereRender = nullptr;	//スフィアレンダー。
};

