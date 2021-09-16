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
	case enStep_InGame: 
		// インゲーム
		break;
	case enStep_Error:
		delete m_onlineTwoPlayerMatchEngine;
		m_onlineTwoPlayerMatchEngine = nullptr;
		m_step = enStep_CharacterSelect;
		m_fontRender.SetText(L"Aボタン : キャラA、Bボタン: キャラB\n");
		DeleteGO(m_actor[0]);
		DeleteGO(m_actor[1]);
		m_charaNo = -1;
		
		break;
	}
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
	Quaternion rot[2];
	rot[0].SetRotationDegY(-90.0f);
	rot[1].SetRotationDegY(90.0f);
	// 自分のプレイヤー番号を取得。
	int playerNo = m_onlineTwoPlayerMatchEngine->GetPlayerNo();
	int otherPlayerNo = m_onlineTwoPlayerMatchEngine->GetOtherPlayerNo();
	// 自分
	m_actor[playerNo]->Init(
		m_onlineTwoPlayerMatchEngine->GetGamePad(playerNo),
		modelPath[m_charaNo],
		pos[playerNo],
		rot[playerNo],
		m_actor[otherPlayerNo]
	);
	// 対戦相手
	m_actor[otherPlayerNo]->Init(
		m_onlineTwoPlayerMatchEngine->GetGamePad(otherPlayerNo),
		modelPath[*(int*)pData],
		pos[otherPlayerNo],
		rot[otherPlayerNo],
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
}