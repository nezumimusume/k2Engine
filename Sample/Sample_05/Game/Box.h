#pragma once
class Box : public IGameObject
{
public:
	Box();
	~Box();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	ModelRender				m_modelRender;				//モデルレンダラー。
	Vector3					m_position;					//座標。
	Vector3					m_scale;					//大きさ。
	Quaternion				m_rotation;					//回転。
	PhysicsStaticObject		m_physicsStaticObject;		//静的物理オブジェクト。
	int						m_lightNumber = 0;
};

