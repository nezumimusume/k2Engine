#pragma once
class BackGround : public IGameObject
{
public:
	bool Start();
	void Update();
	void Render(RenderContext& rc);
private:
	ModelRender				m_modelRender;				//モデルレンダー。
	PhysicsStaticObject		m_physicsStaticObject;		//静的物理オブジェクト。
};

