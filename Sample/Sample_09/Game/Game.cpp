#include "stdafx.h"
#include "Game.h"

#include "Player.h"
#include "Background.h"
#include "GameCamera.h"

Game::Game()
{
	//íœB
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

	m_player = NewGO<Player>(0, "player");
	m_background = NewGO<Background>(0, "background");
	m_gameCamera = NewGO<GameCamera>(0, "gameCamera");

	return true;
}
