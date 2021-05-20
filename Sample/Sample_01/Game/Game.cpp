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
	//アナログスティックによる移動処理。
	//GetLStickXFは、ゲームパッドの横方向の倒れ具合で-1.0～1.0を返してきます。
	//GetLStickYFは、ゲームパッドの縦方向の倒れ具合で-1.0～1.0を返してきます。
	m_position.x += g_pad[0]->GetLStickXF();
	m_position.z += g_pad[0]->GetLStickYF();

	//座標を設定。
	m_modelRender.SetPosition(m_position);

	//モデルの更新。
	m_modelRender.Update();
}

void Game::Render(RenderContext& rc)
{
	//モデルの描画。
	m_modelRender.Draw(rc);
}