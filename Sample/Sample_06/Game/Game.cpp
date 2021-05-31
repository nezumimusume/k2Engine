#include "stdafx.h"
#include "Game.h"

Game::Game()
{

}

Game::~Game()
{

}

bool Game::Start()
{
	m_spriteRender.Init("Assets/sprite/suzuka.dds", 970, 380);
	m_spriteRender.SetScale(Vector3(0.9474f, 0.7432f, 0.0f));
	return true;
}

void Game::Update()
{
	//画像の更新。
	m_spriteRender.Update();
}

void Game::Render(RenderContext& rc)
{
	//画像の描画。
	m_spriteRender.Draw(rc);
}