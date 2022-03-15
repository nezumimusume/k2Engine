#include "stdafx.h"
#include "SphereRender.h"
#include "Game.h"



bool SphereRender::Start()
{
	m_modelRender.Init(
		"Assets/modelData/sample1.tkm",
		nullptr,
		0,
		enModelUpAxisZ,
		true,
		m_maxSphere
	);

	m_game = FindGO<Game>("game");
	return true;
}
void SphereRender::Update()
{
}

void SphereRender::Render(RenderContext& rc)
{
	if (m_game->m_isDrawInstancing == true)
	{
		m_modelRender.Draw(rc);
	}
}