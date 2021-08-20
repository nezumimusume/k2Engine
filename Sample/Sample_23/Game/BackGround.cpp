#include "stdafx.h"
#include "BackGround.h"


bool BackGround::Start()
{
	m_modelRender.Init("Assets/modelData/background.tkm");
	m_modelRender.SetScale(Vector3::One * 0.2f);

	m_modelRender.Update();

	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());

	return true;
}

void BackGround::Update()
{

}

void BackGround::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}