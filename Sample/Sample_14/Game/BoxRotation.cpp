#include "stdafx.h"
#include "BoxRotation.h"

BoxRotation::BoxRotation()
{

}

BoxRotation::~BoxRotation()
{

}

bool BoxRotation::Start()
{
	m_modelRender.Init("Assets/modelData/sample/sample.tkm");
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);
	m_modelRender.SetScale(m_scale);

	m_modelRender.Update();

	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());

	return true;
}

void BoxRotation::Update()
{
	//ボックスを回転させる。
	m_rotation.AddRotationDegY(5.0f);
	m_modelRender.SetRotation(m_rotation);

	m_modelRender.Update();
}

void BoxRotation::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}