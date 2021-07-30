#include "stdafx.h"
#include "SphereRender.h"

bool SphereRender::Start()
{
	m_modelRender.Init(
		"Assets/modelData/sample1.tkm",
		nullptr,
		0,
		enModelUpAxisZ,
		false,
		m_maxSphere
	);
	return true;
}
void SphereRender::Update()
{
}

void SphereRender::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}