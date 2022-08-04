#include "stdafx.h"
#include "BoxMove.h"

BoxMove::BoxMove()
{

}

BoxMove::~BoxMove()
{

}

bool BoxMove::Start()
{
	m_modelRender.Init("Assets/modelData/sample/sample.tkm");
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);
	m_modelRender.SetScale(m_scale);

	m_modelRender.Update();

	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());

	m_defPosition = m_position;	//初期座標を設定。

	return true;
}

void BoxMove::Update()
{
	//ボックスを移動させる。
	if (m_moveflag == false) {
		m_position.y += 1.0f;
	}
	else {
		m_position.y -= 1.0f;
	}

	if (m_position.y <= m_defPosition.y - m_move) {
		m_moveflag = false;
	}
	else if (m_position.y >= m_defPosition.y + m_move) {
		m_moveflag = true;
	}

	m_modelRender.SetPosition(m_position);

	m_modelRender.Update();
}

void BoxMove::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}