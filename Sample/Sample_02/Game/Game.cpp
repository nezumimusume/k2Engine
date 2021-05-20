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
	float degree = 0.0f;
	//上下左右のキー入力による回転処理。
	if (g_pad[0]->IsPress(enButtonRight)) {
		//Y軸周りに2度回す。
		degree = 2.0f;
	}
	else if (g_pad[0]->IsPress(enButtonLeft)) {
		//Y軸周りに-2度回す。
		degree = -2.0f;
	}
	else if (g_pad[0]->IsPress(enButtonUp)) {
		degree = 2.0f;
	}
	else if (g_pad[0]->IsPress(enButtonDown)) {
		degree = -2.0f;
	}
	//角度を加算する。
	m_rotation.AddRotationDegY(degree);

	//アナログスティックによる回転処理。
	Vector3 stick;
	stick.x = -g_pad[0]->GetLStickXF();	//アナログスティックのxの入力量を取得。
	stick.y = g_pad[0]->GetLStickYF();		//アナログスティックのyの入力量を取得。
	stick.z = 0.0f;
	//回転を加算する。
	m_rotation.AddRotationDegY(stick.x * 2.0f);

	//回転を加算する。
	m_rotation.AddRotationDegY(stick.y * 2.0f);

	//クォータニオンを設定する。
	m_modelRender.SetRotation(m_rotation);

	//モデルの更新。
	m_modelRender.Update();
}

void Game::Render(RenderContext& rc)
{
	//モデルの描画。
	m_modelRender.Draw(rc);
}