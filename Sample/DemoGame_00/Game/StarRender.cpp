#include "stdafx.h"
#include "StarRender.h"

bool StarRender::Start()
{
	//���̃��f���͔������`����s���̂ŁAInitTranslucent()���g���B
	m_modelRender.IniTranslucent(
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