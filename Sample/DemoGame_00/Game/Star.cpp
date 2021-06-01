#include "stdafx.h"
#include "Star.h"

bool Star::Start()
{
	m_modelRender.Init("Assets/modelData/star.tkm");
	m_modelRender.SetPosition(m_postiion);
	m_modelRender.SetScale(m_scale);
	return true;
}

void Star::Update()
{
	m_modelRender.Update();
}

void Star::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}
