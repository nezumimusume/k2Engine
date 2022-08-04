#pragma once
class BoxMove : public IGameObject
{
public:
	BoxMove();
	~BoxMove();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	ModelRender				m_modelRender;				//モデルレンダラー。
	Vector3					m_position;					//座標。
	Vector3					m_scale;					//大きさ。
	Quaternion				m_rotation;					//回転。
	PhysicsStaticObject		m_physicsStaticObject;		//静的物理オブジェクト。

	Vector3					m_defPosition;				//初期座標。
	const float				m_move = 20.0f;				//上下移動量の上限。
	bool					m_moveflag = false;			//上下移動のフラグ。
};

