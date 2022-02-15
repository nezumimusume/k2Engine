#include "stdafx.h"
#include "Game.h"


bool Game::Start()
{
	g_renderingEngine->DisableRaytracing();

	m_modelRender.Init("Assets/modelData/stage/road.tkm");

	
	return true;
}

void Game::Update()
{
	m_modelRender.Update();
}

void Game::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}