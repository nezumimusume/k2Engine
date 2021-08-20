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

	if (m_skyCubeType == enSkyCubeType_Night
		|| m_skyCubeType == enSkyCubeType_Wild_Night
		|| m_skyCubeType == enSkyCubeType_NightToon
		|| m_skyCubeType == enSkyCubeType_NightToon_2
		) {


		m_skyCube->SetLuminance(0.1f);
		Vector3 ligColor, ligDir;
		ligColor.x = 0.6f;
		ligColor.y = 0.6f;
		ligColor.z = 0.6f;
		ligDir.x = 1.0f;
		ligDir.y = -1.0f;
		ligDir.z = -1.0f;
		ligDir.Normalize();
		g_sceneLight->SetDirectionLight(0, ligDir, ligColor);
	}
	else if (m_skyCubeType == enSkyCubeType_Snow) {

		m_skyCube->SetLuminance(1.0f);
	}
	else if (m_skyCubeType == enSkyCubeType_Snow_2) {

		m_skyCube->SetLuminance(1.0f);
	}
	else if (m_skyCubeType == enSkyCubeType_Wild) {

		m_skyCube->SetLuminance(0.5f);
	}
	else if (m_skyCubeType == enSkyCubeType_Wild_2) {

		m_skyCube->SetLuminance(0.5f);
	}
	else if (m_skyCubeType == enSkyCubeType_Grass) {

		m_skyCube->SetLuminance(0.5f);
	}
	else if (m_skyCubeType == enSkyCubeType_Euro) {

		m_skyCube->SetLuminance(0.5f);
	}
	else if (m_skyCubeType == enSkyCubeType_DayToon) {
		m_skyCube->SetLuminance(2.0f);
	}
	else if (m_skyCubeType == enSkyCubeType_DayToon_2) {
		m_skyCube->SetLuminance(0.2f);
	}
	else if (m_skyCubeType == enSkyCubeType_DayToon_3) {
		m_skyCube->SetLuminance(0.8f);
	}
	else if (m_skyCubeType == enSkyCubeType_DayToon_4) {
		m_skyCube->SetLuminance(0.5f);
	}
	else if (m_skyCubeType == enSkyCubeType_SunriseToon) {
		m_skyCube->SetLuminance(0.5f);
	}
	else if (m_skyCubeType == enSkyCubeType_SpaceToon
		|| m_skyCubeType == enSkyCubeType_SpaceToon_2) {
		m_skyCube->SetLuminance(0.2f);
	}

	m_skyCube->SetType((EnSkyCubeType)m_skyCubeType);
	// 環境光の計算のためのIBLテクスチャをセットする。
	g_sceneLight->SetIBLTextureForAmbient(m_skyCube->GetTextureFilePath(), 0.1f);
}
bool Game::Start()
{
	g_camera3D->SetPosition({ 0.0f, 100.0f, -600.0f });
	g_camera3D->SetFar(40000.0f);
	
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
