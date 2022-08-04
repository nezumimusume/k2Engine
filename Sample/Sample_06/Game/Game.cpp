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
	//�摜�̍X�V�B
	m_spriteRender.Update();
}

void Game::Render(RenderContext& rc)
{
	//�摜�̕`��B
	m_spriteRender.Draw(rc);
}