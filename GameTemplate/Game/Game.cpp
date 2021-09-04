#include "stdafx.h"
#include "Game.h"


bool Game::Start()
{
	m_modelRender.Init("Assets/modelData/test.tkm");
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