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
	ModelRender			m_modelRender;							//モデルレンダラー。
	PhysicsStaticObject m_phyStaticObject;						//静的物理オブジェクト。
	Vector3				m_position;								//座標。
	Quaternion			m_rotation = Quaternion::Identity;		//回転。
	Vector3				m_scale = Vector3::One;					//大きさ。
};