#include "stdafx.h"
#include "Game.h"

//スカイキューブ。お空。
#include "nature/SkyCube.h"

#include "Player.h"
#include "Background.h"
#include "GameCamera.h"
#include "Star.h"
#include "Enemy.h"
#include "Title.h"

#include "Fade.h"

#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"

namespace
{
	const float TIME_LIMIT = 90.0f;
	const Vector3	STAR_COUNT_FONT_POSITION = Vector3(-640.0f, 250.0f, 0.0f);
	const float STAR_COUNT_FONT_CENTER_POSITION_X = -170.0f;
	const float STAR_COUNT_FONT_POSITION_MOVE_SPEED = 350.0f;

	const float PRESS_A_BUTTON_TEXTURE_SCALE = 0.839f;
	const Vector3 PRESS_A_BUTTON_TEXTURE_POSITION = Vector3(0.0f, -280.0f, 0.0f);
}

Game::Game()
{
	
}

Game::~Game()
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
	const auto& stars = FindGOs<Star>("star");
	for (auto star : stars)
	{
		DeleteGO(star);
	}
	DeleteGO(m_bgm);
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
			m_player->SetStartPosition(objData.position);
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
			int number = _wtoi(&objData.name[5]);
			//エネミーの番号によって、パスデータを読み込む。
			enemy->LoadPath(number);
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

	m_timer = TIME_LIMIT;
	m_starCountFontPosition = STAR_COUNT_FONT_POSITION;
	m_pressA.Init("Assets/sprite/press_a_button.dds", 1024, 256);
	m_pressA.SetScale(Vector3(PRESS_A_BUTTON_TEXTURE_SCALE, PRESS_A_BUTTON_TEXTURE_SCALE, 1.0f));
	m_pressA.SetPosition(PRESS_A_BUTTON_TEXTURE_POSITION);
	m_pressA.Update();

	g_soundEngine->ResistWaveFileBank(2, "Assets/sound/normalBGM.wav");
	m_bgm = NewGO<SoundSource>(0);
	m_bgm->Init(2);
	m_bgm->Play(true);
	return true;
}

void Game::NotifyGameOver()
{
	m_player->NotifyGameOver();
	m_gameState = enGameState_GameOver;
}

void Game::NotifyGameClear()
{
	m_player->NotifyGameClear();
	m_gameCamera->NotifyGameClear();
	m_gameState = enGameState_GameClear;
}

void Game::NotifyGameClearIdle()
{
	m_gameState = enGameState_GameClear_Idle;
}

void Game::NotifyReStart()
{
	m_player->ReStart();
	m_gameCamera->ReStart();
	m_gameState = enGameState_DuringGamePlay;
}

void Game::CountTimer()
{
	if (m_gameState != enGameState_GameClear)
	{
		m_timer -= g_gameTime->GetFrameDeltaTime();
		if (m_timer <= 0.0f)
		{
			m_timer = 0.0f;
			//ゲームオーバーじゃ無かったら。
			if (m_player->GetPlayerState() != Player::enPlayerState_GameOver)
			{
				NotifyGameClear();
			}
		}
	}
}

void Game::Update()
{
	if (m_gameState == enGameState_GameClear_Idle)
	{

		if (m_isWaitFadeout) {
			if (!m_fade->IsFade()) {
				NewGO<Title>(0, "title");
				DeleteGO(this);
			}
		}
		else {
			if (g_pad[0]->IsTrigger(enButtonA)) {
				m_isWaitFadeout = true;
				m_fade->StartFadeOut();
			}
		}
		return;
	}

	UpdateFont();
	CountTimer();
}

void Game::UpdateFont()
{
	if (m_gameState == enGameState_GameClear)
	{
		m_starCountFontPosition.x += STAR_COUNT_FONT_POSITION_MOVE_SPEED * g_gameTime->GetFrameDeltaTime();
		if (m_starCountFontPosition.x >= STAR_COUNT_FONT_CENTER_POSITION_X)
		{
			m_starCountFontPosition.x = STAR_COUNT_FONT_CENTER_POSITION_X;
		}
	}

	wchar_t wcsbuf[256];
	swprintf_s(wcsbuf, 256, L"STAR %04d", int(m_starCount));

	//表示するテキストを設定。
	m_starCountFont.SetText(wcsbuf);
	m_starCountFont.SetPosition(m_starCountFontPosition);
	m_starCountFont.SetScale(1.5f);
	m_starCountFont.SetColor(Vector4(1.0f, 1.0f, 0.0f, 1.0f));

	wchar_t text[256];
	int minute = (int)m_timer / 60;
	int sec = (int)m_timer % 60;
	swprintf_s(text, 256, L"%02d:%02d", minute, sec);
	m_timerFont.SetText(text);
	m_timerFont.SetPosition(Vector3(-640.0f, 350.0f, 0.0f));
	m_timerFont.SetScale(1.5f);
	m_timerFont.SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
}

void Game::Render(RenderContext& rc)
{
	m_starCountFont.Draw(rc);
	m_timerFont.Draw(rc);

	if (m_gameState == enGameState_GameClear_Idle)
	{
		m_pressA.Draw(rc);
	}
}