#include "stdafx.h"
#include "StarRender.h"

bool StarRender::Start()
{
	//フォワードレンダリング。
	m_modelRender.InitForwardRendering(
		"Assets/modelData/star.tkm",
		nullptr,
		0,
		enModelUpAxisZ,
		false,
		m_maxStar,
		AlphaBlendMode_Trans
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