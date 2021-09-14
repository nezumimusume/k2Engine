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
	float half_x = FRAME_BUFFER_W * 0.45f;
	float half_y = FRAME_BUFFER_H * 0.45f;
	m_fontRender.SetText(L"Aボタン : キャラA、Bボタン: キャラB\n");
	m_fontRender.SetPosition({ -half_x , half_y, 0.0f});
	m_positionRender[0].SetPosition({ -half_x, half_y - 50.0f, 0.0f });
	m_positionRender[1].SetPosition({ -half_x, half_y - 100.0f, 0.0f });
	return true;
}

void Game::Update()
{
	if (m_onlineTwoPlayerMatchEngine) {
		m_onlineTwoPlayerMatchEngine->Update();
	}
	switch (m_step) {
	case enStep_CharacterSelect: {
		int charaNo = -1;
		if (g_pad[0]->IsPress(enButtonA)) {
			charaNo = 0;
		}
		else if (g_pad[0]->IsPress(enButtonB)) {
			charaNo = 1;
		}
		if (charaNo != -1) {
			// キャラが選ばれたのオンラインマッチをスタートする。
			m_onlineTwoPlayerMatchEngine = new SyncOnlineTwoPlayerMatchEngine;
			m_onlineTwoPlayerMatchEngine->Init(
				L"41b72c35-96cd-4cab-ab81-cc1313e36213",
				L"1.0",
				&charaNo,
				sizeof(int),
				[&](void* pData, int size) { OnAllPlayerJoined(pData, size); },
				[&]() { OnAllPlayerStartGame(); },
				[&]() { OnError(); }
			);
			// すべてのプレイヤーが揃うのを待つ。
			m_fontRender.SetText(L"WaitAllPlayerJoined!!");
			m_step = enStep_WaitAllPlayerJoined;
		}
	}break;
		break;
	case enStep_InGame: {
		for (int i = 0; i < 2; i++) {
			auto& pad = m_onlineTwoPlayerMatchEngine->GetGamePad(i);
			pos[i].x += pad.GetLStickXF() * 4.0f;
			pos[i].y += pad.GetLStickYF() * 4.0f;
			m_modelRender[i].SetPosition(pos[i]);
			m_modelRender[i].Update();
			
		}
		wchar_t text[256];
		swprintf_s(text, L"1P x : %f, y : %f, z : %f\n", pos[0].x, pos[0].y, pos[0].z);
		m_positionRender[0].SetText(text);
		swprintf_s(text, L"2P x : %f, y : %f, z : %f\n", pos[1].x, pos[1].y, pos[1].z);
		m_positionRender[1].SetText(text);
	}break;
	case enStep_Error:
		delete m_onlineTwoPlayerMatchEngine;
		m_onlineTwoPlayerMatchEngine = nullptr;
		m_step = enStep_CharacterSelect;
		m_fontRender.SetText(L"Aボタン : キャラA、Bボタン: キャラB\n");
		pos[0] = g_vec3Zero;
		pos[1] = g_vec3Zero;
		break;
	}
}
void Game::OnAllPlayerJoined(void* pData, int size)
{
	// すべてのプレイヤーが揃った。
	m_modelRender[0].Init("Assets/modelData/unityChan.tkm");
	m_modelRender[1].Init("Assets/modelData/unityChan.tkm");
	// ロードが終わってゲーム開始可能になったことを通知する。
	m_onlineTwoPlayerMatchEngine->NotifyPossibleStartPlayGame();
	// ほかのプレイヤーがゲーム開始可能になるまで待つ。
	m_step = enStep_WaitAllPlayerStartGame;

	m_fontRender.SetText(L"WaitAllPlayerStartGame!!");
}

void Game::OnAllPlayerStartGame()
{
	// すべてのプレイヤーがゲーム開始可能になったので、ゲームスタート！
	m_fontRender.SetText(L"InGame!!");
	m_step = enStep_InGame;

}
void Game::OnError()
{
	MessageBoxA(nullptr, "通信エラーが起きました。", "エラー", MB_OK);
	m_step = enStep_Error;
}
void Game::Render(RenderContext& rc)
{
	m_fontRender.Draw(rc);
	switch (m_step) {
	case enStep_InGame:
		for (int i = 0; i < 2; i++) {
			m_modelRender[i].Draw(rc);
			m_positionRender[i].Draw(rc);
		}
		break;
	}
}