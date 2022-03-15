#pragma once
class SphereRender;
class Game;

class Sphere : public IGameObject
{
public:
	Sphere();
	~Sphere();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	int						m_instanceNo = 0;			// インスタンス番号。
	Vector3					m_position;					// 座標。
	Vector3					m_scale;					// 大きさ。
	Quaternion				m_rotation;					// 回転。
	SphereRender*			m_sphereRender = nullptr;	// スフィアレンダー。
	ModelRender				m_modelRender;				// モデルレンダー。
	Game*					m_game = nullptr;
};

