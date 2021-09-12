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
	m_fontRender.SetText(L"Aボタン : キャラA、Bボタン: キャラB\n");
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
	case enStep_InGame:
		break;
	case enStep_Error:
		delete m_onlineTwoPlayerMatchEngine;
		m_onlineTwoPlayerMatchEngine = nullptr;
		m_step = enStep_CharacterSelect;
		m_fontRender.SetText(L"Aボタン : キャラA、Bボタン: キャラB\n");
		break;
	}
}
void Game::OnAllPlayerJoined(void* pData, int size)
{
	// すべてのプレイヤーが揃った。
	m_modelRender->Init("Assets/modelData/unityChan.tkm");
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
		m_modelRender->Draw(rc);
		break;
	}
}