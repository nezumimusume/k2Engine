#include "stdafx.h"
#include "StarRender.h"

bool StarRender::Start()
{
	m_modelRender.InitForwardRendering(
		"Assets/modelData/star.tkm",
		nullptr,
		0,
		enModelUpAxisZ,
		false,
		m_maxStar
	);
	return true;
}
void StarRender::Update()
{
}
void StarRender::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}