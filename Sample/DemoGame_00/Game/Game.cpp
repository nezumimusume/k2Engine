#include "stdafx.h"
#include "Game.h"

#include "nature/SkyCube.h"

#include "Player.h"
#include "Background.h"
#include "GameCamera.h"
#include "Star.h"

Game::Game()
{
	//削除。
	DeleteGO(m_player);
	DeleteGO(m_background);
	DeleteGO(m_gameCamera);
}

Game::~Game()
{

}

bool Game::Start()
{
	g_camera3D->SetPosition({ 0.0f, 100.0f, -600.0f });
	
	m_gameCamera = NewGO<GameCamera>(0, "gameCamera");

	//m_skyCube = NewGO<SkyCube>(0, "skycube");

	//レベルを構築する。
	m_levelRender.Init("Assets/level3D/stage.tkl", [&](LevelObjectData& objData) {
		if (objData.EqualObjectName(L"unityChan") == true) {
			//Unityちゃん。
			//プレイヤーのインスタンスを生成する。
			m_player = NewGO<Player>(0, "player");
			m_player->m_position = objData.position;
			//trueにすると、レベルの方でモデルが読み込まれない。
			return true;
		}
		else if (objData.EqualObjectName(L"background") == true) {
			m_background = NewGO<Background>(0, "background");
			m_background->m_position = objData.position;
			m_background->m_scale = objData.scale;
			m_background->m_rotation = objData.rotation;
			return true;
		}
		else if (objData.EqualObjectName(L"star") == true) {
			//auto star = NewGO<Star>(0, "star");
			//star->m_postiion = objData.position;
			return true;
		}
		return false;
	});

	return true;
}
