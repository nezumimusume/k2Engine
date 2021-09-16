#include "stdafx.h"
#include "Game.h"
#include "Actor.h"

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
		
		if (g_pad[0]->IsPress(enButtonA)) {
			m_charaNo = 0;
		}
		else if (g_pad[0]->IsPress(enButtonB)) {
			m_charaNo = 1;
		}
		if (m_charaNo != -1) {
			// キャラが選ばれたのオンラインマッチをスタートする。
			m_onlineTwoPlayerMatchEngine = new SyncOnlineTwoPlayerMatchEngine;
			m_onlineTwoPlayerMatchEngine->Init(
				L"41b72c35-96cd-4cab-ab81-cc1313e36213",
				L"1.0",
				&m_charaNo,
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
	case enStep_WaitAllPlayerJoined:
		// 全てのプレイヤーがルームにジョインするのを待っている。
		break;
	case enStep_WaitAllPlayerStartGame:
		// 全てのプレイヤーがゲーム開始可能になるのを待っている。
		break;
	case enStep_InGame: {
		// インゲーム
		for (int i = 0; i < 2; i++) {
			const Vector3& actorPos = m_actor[i]->GetPosition();
			wchar_t text[256];
			swprintf_s(text, L"1P x : %f, y : %f, z : %f\n", actorPos.x, actorPos.y, actorPos.z);
			m_positionRender[i].SetText(text);
		}
		int playerNo = m_onlineTwoPlayerMatchEngine->GetPlayerNo();
		int otherPlayerNo = m_onlineTwoPlayerMatchEngine->GetOtherPlayerNo();
		if (m_actor[playerNo]->IsDowned()) {
			MessageBoxA(nullptr, "あなたの負け", "結果", MB_OK);
			ReturnCharacterSelect();
		}
		if (m_actor[otherPlayerNo]->IsDowned()) {
			MessageBoxA(nullptr, "あなたの勝ち", "結果", MB_OK);
			ReturnCharacterSelect();
		}
	}break;
	case enStep_Error:
		ReturnCharacterSelect();
		
		break;
	}
}
void Game::ReturnCharacterSelect()
{
	delete m_onlineTwoPlayerMatchEngine;
	m_onlineTwoPlayerMatchEngine = nullptr;
	m_step = enStep_CharacterSelect;
	m_fontRender.SetText(L"Aボタン : キャラA、Bボタン: キャラB\n");
	DeleteGO(m_actor[0]);
	DeleteGO(m_actor[1]);
	m_charaNo = -1;
}
void Game::OnAllPlayerJoined(void* pData, int size)
{
	// すべてのプレイヤーが揃った。
	m_actor[0] = NewGO<Actor>(0, "Actor");
	m_actor[1] = NewGO<Actor>(0, "Actor");
	const char* modelPath[] = {
		"Assets/modelData/human/human.tkm",
		"Assets/modelData/enemy/enemy.tkm",
	};
	const Vector3 pos[] = {
		{100.0f, 0.0f, 0.0f},		// 1Pの初期座標
		{-100.0f, 0.0f, 0.0f},		// 2Pの初期座標。
	};
	float rotAngle[] = {
		-90.0f,
		90.0f
	};
	
	// 自分のプレイヤー番号を取得。
	int playerNo = m_onlineTwoPlayerMatchEngine->GetPlayerNo();
	int otherPlayerNo = m_onlineTwoPlayerMatchEngine->GetOtherPlayerNo();
	// 自分
	m_actor[playerNo]->Init(
		m_onlineTwoPlayerMatchEngine->GetGamePad(playerNo),
		modelPath[m_charaNo],
		pos[playerNo],
		rotAngle[playerNo],
		m_actor[otherPlayerNo]
	);
	// 対戦相手
	m_actor[otherPlayerNo]->Init(
		m_onlineTwoPlayerMatchEngine->GetGamePad(otherPlayerNo),
		modelPath[*(int*)pData],
		pos[otherPlayerNo],
		rotAngle[otherPlayerNo],
		m_actor[playerNo]
	);

	
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
		for (auto& fontRender : m_positionRender) {
			fontRender.Draw(rc);
		}
		break;
	}
}