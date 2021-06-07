#include "stdafx.h"
#include "Game.h"

//スカイキューブ。お空。
#include "nature/SkyCube.h"

#include "Player.h"
#include "Background.h"
#include "GameCamera.h"
#include "Star.h"
#include "Enemy.h"

#include "Fade.h"

Game::Game()
{
	//削除。
	DeleteGO(m_player);
	DeleteGO(m_background);
	DeleteGO(m_gameCamera);
	DeleteGO(m_skyCube);
	for (auto enemy : m_enemys)
	{
		DeleteGO(enemy);
	}
}

Game::~Game()
{

}

bool Game::Start()
{
	g_camera3D->SetPosition({ 0.0f, 100.0f, -600.0f });
	g_camera3D->SetFar(40000.0f);
	
	m_gameCamera = NewGO<GameCamera>(0, "gameCamera");

	m_skyCube = NewGO<SkyCube>(0, "skycube");

	//レベルを構築する。
	m_levelRender.Init("Assets/level3D/stage.tkl", [&](LevelObjectData& objData) {
		if (objData.EqualObjectName(L"unityChan") == true) {
			//Unityちゃん。
			//プレイヤーのインスタンスを生成する。
			m_player = NewGO<Player>(0, "player");
			m_player->SetPosition(objData.position);
			//trueにすると、レベルの方でモデルが読み込まれない。
			return true;
		}
		else if (objData.EqualObjectName(L"background") == true) {
			m_background = NewGO<Background>(0, "background");
			m_background->SetPosition(objData.position);
			m_background->SetScale(objData.scale);
			m_background->SetRotation(objData.rotation);
			return true;
		}
		else if (objData.ForwardMatchName(L"enemy") == true) {
			auto enemy = NewGO<Enemy>(0, "enemy");
			enemy->SetPosition(objData.position);
			enemy->SetScale(objData.scale);
			m_enemys.push_back(enemy);
			return true;
		}
		else if (objData.ForwardMatchName(L"star") == true) {
			auto star = NewGO<Star>(0, "star");
			star->SetPosition(objData.position);
			return true;
		}

		return true;
	});

	m_fade = FindGO<Fade>("fade");
	m_fade->StartFadeIn();

	
	return true;
}

void Game::Update()
{
	UpdateFont();
	
	m_timer -= g_gameTime->GetFrameDeltaTime();
}

void Game::UpdateFont()
{
	wchar_t wcsbuf[256];
	swprintf_s(wcsbuf, 256, L"STAR %04d", int(m_starCount));

	//表示するテキストを設定。
	m_starCountFont.SetText(wcsbuf);
	m_starCountFont.SetPosition(Vector3(-640.0f, 250.0f, 0.0f));
	m_starCountFont.SetScale(1.5f);

	wchar_t text[256];
	int minute = (int)m_timer / 60;
	int sec = (int)m_timer % 60;
	swprintf_s(text, 256, L"%02d:%02d", minute, sec);
	m_timerFont.SetText(text);
	m_timerFont.SetPosition(Vector3(-640.0f, 350.0f, 0.0f));
	m_timerFont.SetScale(1.5f);
}

void Game::Render(RenderContext& rc)
{
	m_starCountFont.Draw(rc);
	m_timerFont.Draw(rc);
}