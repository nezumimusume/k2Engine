#include "stdafx.h"
#include "Game.h"

//スカイキューブ。お空。
#include "nature/SkyCube.h"

#include "Player.h"
#include "Background.h"
#include "GameCamera.h"
#include "Star.h"
#include "Pyramid.h"
#include "Enemy.h"
#include "Title.h"

#include "Fade.h"
#include "StarRender.h"
#include "PyramidRender.h"

#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"
#include "graphics/RenderingEngine.h"

namespace
{
	const float TIME_LIMIT = 1000.0f;
	const Vector3	STAR_COUNT_FONT_POSITION = Vector3(-960.0f, 400.0f, 0.0f);
	const float STAR_COUNT_FONT_CENTER_POSITION_X = 50.0f;
	const float STAR_COUNT_FONT_POSITION_MOVE_SPEED = 430.0f;

	const float PRESS_A_BUTTON_TEXTURE_SCALE = 1.0f;
	const Vector3 PRESS_A_BUTTON_TEXTURE_POSITION = Vector3(0.0f, -400.0f, 0.0f);

	const Vector3 POINTLIGHT_COLOR = Vector3(4.0f, 1.0f, 1.0f);
	const float POINTLIGHT_RANGE = 220.0f;
	const float POINTLIGHT_ATTEN_POW = 1.5f;
	const float SPOTLIGHT_ATTEN_POW = 0.65f;
}

Game::Game()
{
	// g_renderingEngine->DisableRaytracing();
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
	auto starRender = FindGO<StarRender>("StarRender");
	DeleteGO(starRender);

	const auto& pyramids = FindGOs<Pyramid>("pyramid");
	for (auto pyramid : pyramids)
	{
		DeleteGO(pyramid);
	}
	auto pyramidRender = FindGO<PyramidRender>("PyramidRender");
	DeleteGO(pyramidRender);

	//ポイントライトを削除。
	for (auto pt : m_pointLightList) {
		delete pt;
	}
	for (auto spt : m_spotLightList) {
		delete spt;
	}
	for (auto vspt : m_volumeSpotLightList){
		delete vspt;
	}
	for (auto vpt : m_volumePointLightList) {
		delete vpt;
	}
}
void Game::InitSky()
{
	// 現在の空を破棄。
	DeleteGO(m_skyCube);
	// g_renderingEngine->DisableTonemap();

	m_skyCube = NewGO<SkyCube>(0, "skycube");
	m_skyCube->SetType((EnSkyCubeType)m_skyCubeType);
	
	// 環境光の計算のためのIBLテクスチャをセットする。
	g_renderingEngine->SetAmbientByIBLTexture(m_skyCube->GetTextureFilePath(), 0.1f);
}
bool Game::Start()
{
	//PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();
	
	g_renderingEngine->NotifyChangeScene(0.5f);
	
	//カスケードシャドウの近影エリア率を
	g_renderingEngine->SetCascadeNearAreaRates(0.1f, 0.2f, 0.3f);
	g_camera3D->SetPosition({ 0.0f, 100.0f, -600.0f });
	g_camera3D->SetFar(40000.0f);
	
	m_gameCamera = NewGO<GameCamera>(0, "gameCamera");

	InitSky();
	
	int numStar = 0;
	int numPyramid = 0;
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
			star->SetInstanceNo(numStar);
			numStar++;
			return true;
		}
		else if (objData.ForwardMatchName(L"pyramid") == true) {
			auto pyramid = NewGO<Pyramid>(0, "pyramid");
			pyramid->SetPosition(objData.position);
			pyramid->SetScale(objData.scale);
			pyramid->SetInstanceNo(numPyramid);
			numPyramid++;
			return true;
		}
		else if (objData.ForwardMatchName(L"pointlight") == true) {
			auto pointLight = new PointLight;
			pointLight->Init();
			pointLight->SetPosition(objData.position);
			pointLight->SetColor(POINTLIGHT_COLOR);
			pointLight->SetRange(POINTLIGHT_RANGE);
			pointLight->SetAffectPowParam(POINTLIGHT_ATTEN_POW);
			m_pointLightList.push_back(pointLight);
#ifdef USE_VOLUME_SPOTLIGHT
			VolumePointLight* volumePointLight = new VolumePointLight;
			volumePointLight->Init(*pointLight);
			m_volumePointLightList.push_back(volumePointLight);
#endif
			/*SpotLight* spotLight = new SpotLight;
			spotLight->Init();
			spotLight->SetPosition(objData.position);
			spotLight->SetColor(POINTLIGHT_COLOR);
			spotLight->SetRange(POINTLIGHT_RANGE);
			spotLight->SetRangeAffectPowParam(SPOTLIGHT_ATTEN_POW);
			spotLight->SetAngleAffectPowParam(1.0f);
			spotLight->SetDirection(g_vec3Down);
			spotLight->SetAngle(Math::DegToRad(40.0f));
			m_spotLightList.push_back(spotLight);
			*/
			/*VolumeSpotLight* volumeSpotLight = new VolumeSpotLight();
			volumeSpotLight->Init(*spotLight);
			m_volumeSpotLightList.push_back(volumeSpotLight);*/
			return true;
		}

		return true;
	});

	//Starレンダラーを作成。
	auto starRender = NewGO<StarRender>(0, "StarRender");
	starRender->SetMaxStar(numStar);

	auto pyramidRender = NewGO<PyramidRender>(0, "PyramidRender");
	pyramidRender->SetMaxPyramid(numPyramid);

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
	// ポイントライトの更新。
	for (auto pt : m_pointLightList) {
		pt->Update();
	}
	// スポットライトの更新。
	for (auto spotLight : m_spotLightList) {
		spotLight->Update();
	}
	// ボリュームライトの更新。
	for (auto volumeSpotLight : m_volumeSpotLightList) {
		volumeSpotLight->Update();
	}
	for (auto volumePointLight : m_volumePointLightList) {
		volumePointLight->Update();
	}
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
	//レベルをマップチップをアップデート。
	m_levelRender.Update();
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
	m_starCountFont.SetScale(2.3f);
	m_starCountFont.SetColor(Vector4(1.0f, 1.0f, 0.0f, 1.0f));

	wchar_t text[256];
	int minute = (int)m_timer / 60;
	int sec = (int)m_timer % 60;
	swprintf_s(text, 256, L"%02d:%02d", minute, sec);
	m_timerFont.SetText(text);
	m_timerFont.SetPosition(Vector3(-960.0f, 530.0f, 0.0f));
	m_timerFont.SetScale(2.3f);
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

	//マップチップを描画。
	m_levelRender.Draw(rc);
}