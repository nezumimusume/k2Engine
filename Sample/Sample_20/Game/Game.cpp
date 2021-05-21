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
	if (m_rotateMode == 0) {
		//角度を加算する。
		m_rotation.AddRotationDegY(1.0f);
		//合成したクォータニオンをモデルレンダラーに伝える。
		m_modelRender.SetRotation(m_rotation);
	}
	else if (m_rotateMode == 1) {
		//スティックで回転させるモード。
		//回転を加算する。
		m_rotation.AddRotationDegY(g_pad[0]->GetLStickXF());
		m_rotation.AddRotationDegX(g_pad[0]->GetLStickYF());
		//合成したクォータニオンをモデルレンダラーに伝える。
		m_modelRender.SetRotation(m_rotation);
	}
	if (g_pad[0]->IsTrigger(enButtonA) == true) {
		//Aボタンを押したら、回転モードを変更する。
		if (m_rotateMode == 0) {
			m_rotateMode = 1;
		}
		else if (m_rotateMode == 1) {
			m_rotateMode = 0;
		}
	}
	//モデルの更新。
	m_modelRender.Update();
}


void Game::Render(RenderContext& rc)
{
	//モデルの描画。
	m_modelRender.Draw(rc);
}