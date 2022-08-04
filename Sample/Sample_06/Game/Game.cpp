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
	m_spriteRender.Init("Assets/sprite/windowsSprite.dds", 1280, 720);
	m_spriteRender.SetScale(Vector3(1.0f, 1.0f, 0.0f));
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