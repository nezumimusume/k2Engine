#include "stdafx.h"
#include "Game.h"

//スカイキューブ。お空。
#include "nature/SkyCube.h"

#include "Player.h"
#include "GameCamera.h"
#include "Sphere.h"

#include "graphics/RenderingEngine.h"

Game::Game()
{
	
}

Game::~Game()
{
	//削除。
	DeleteGO(m_player);
	DeleteGO(m_gameCamera);
	DeleteGO(m_skyCube);
	DeleteGO(m_sphere);
}
void Game::InitSky()
{
	// 現在の空を破棄。
	DeleteGO(m_skyCube);
	m_skyCube = NewGO<SkyCube>(0, "skycube");

	m_skyCube->SetLuminance(1.0f);
	m_skyCube->SetType((EnSkyCubeType)m_skyCubeType);

	// 環境光の計算のためのIBLテクスチャをセットする。
	g_renderingEngine->SetAmbientByIBLTexture(m_skyCube->GetTextureFilePath(), 1.0f);
	// 環境日光の影響が分かりやすいように、ディレクションライトはオフに。
	g_renderingEngine->SetDirectionLight(0, g_vec3Zero, g_vec3Zero);
}
bool Game::Start()
{	
	InitSky();
	
	m_gameCamera = NewGO<GameCamera>(0, "gameCamera");
	m_player = NewGO<Player>(0, "player");
	m_sphere = NewGO<Sphere>(0, "sphere");

	return true;
}


void Game::Update()
{
	//お空を変更してる。
	if (g_pad[0]->IsTrigger(enButtonRight)) {
		m_skyCubeType++;
		m_skyCubeType = m_skyCubeType % enSkyCubeType_Num;
		InitSky();
	}
	if (g_pad[0]->IsTrigger(enButtonLeft)) {
		m_skyCubeType--;
		if (m_skyCubeType < 0) {
			m_skyCubeType = enSkyCubeType_Num - 1;
		}
		InitSky();
	}
	
}
