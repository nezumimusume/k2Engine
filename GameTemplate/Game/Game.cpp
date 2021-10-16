#include "stdafx.h"
#include "Game.h"
#include "geometry/BSP.h"


bool Game::Start()
{
	m_modelRender.Init("Assets/modelData/unityChan.tkm");
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