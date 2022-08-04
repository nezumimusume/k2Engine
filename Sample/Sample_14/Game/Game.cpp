#include "stdafx.h"
#include "Game.h"

#include "Player.h"
#include "GameCamera.h"

#include "BoxMove.h"
#include "BoxRotation.h"

Game::Game()
{

}

Game::~Game()
{
	//削除。
	DeleteGO(m_player);
	DeleteGO(m_gameCamera);
	//プッシュしたボックスを削除していく。
	for (auto box : m_boxmoves)
	{
		DeleteGO(box);
	}
	for (auto box : m_boxrots)
	{
		DeleteGO(box);
	}
}

bool Game::Start()
{
	g_camera3D->SetPosition({ 0.0f, 100.0f, -600.0f });

	m_gameCamera = NewGO<GameCamera>(0, "gameCamera");

	m_player = NewGO<Player>(0, "player");

	//レベルを構築する。
	m_levelRender.Init("Assets/level/sample.tkl", [&](LevelObjectData& objData) {

		if (objData.EqualObjectName(L"Box_Move") == true) {

			auto box = NewGO<BoxMove>(0);
			//配置座標、スケール、回転を取得する。
			box->m_position = objData.position;
			box->m_scale = objData.scale;
			box->m_rotation = objData.rotation;
			//後で削除するために、ボックスをプッシュしておく。
			m_boxmoves.push_back(box);

			//trueにすると、レベルの方でモデルが読み込まれない。
			return true;
		}

		if (objData.EqualObjectName(L"Box_Rotation") == true) {

			auto box = NewGO<BoxRotation>(0);
			//配置座標、スケール、回転を取得する。
			box->m_position = objData.position;
			box->m_scale = objData.scale;
			box->m_rotation = objData.rotation;
			//後で削除するために、ボックスをプッシュしておく。
			m_boxrots.push_back(box);

			//trueにすると、レベルの方でモデルが読み込まれない。
			return true;
		}

		return false;
	});

	return true;
}

void Game::Update()
{

}

void Game::Render(RenderContext& rc)
{
	//レベルで読み込んだモデルを表示させる。
	m_levelRender.Draw(rc);
}
