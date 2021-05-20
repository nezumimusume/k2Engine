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
	//モデルを読み込む。
	m_modelRender.Init("Assets/modelData/unityChan.tkm");

	return true;
}

void Game::Update()
{
	//モデルの更新。
	m_modelRender.Update();
}

void Game::Render(RenderContext& rc)
{
	//モデルの描画。
	m_modelRender.Draw(rc);
}