#pragma once

class Background : public IGameObject
{
public:
	//////////////////////////////////////
	// メンバ関数。
	//////////////////////////////////////
	Background();
	~Background();
	bool Start();
	void Render(RenderContext& rc);

	////////////////////////////////////
	// メンバ変数とか
	////////////////////////////////////
	ModelRender m_modelRender;					//モデルレンダラー。
	PhysicsStaticObject m_phyStaticObject;		//静的物理オブジェクト。
};