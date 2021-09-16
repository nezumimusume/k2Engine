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
	m_fontRender.SetText(L"A�{�^�� : �L����A�AB�{�^��: �L����B\n");
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
			// �L�������I�΂ꂽ�̃I�����C���}�b�`���X�^�[�g����B
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
			// ���ׂẴv���C���[�������̂�҂B
			m_fontRender.SetText(L"WaitAllPlayerJoined!!");
			m_step = enStep_WaitAllPlayerJoined;
		}
	}break;
		break;
	case enStep_WaitAllPlayerJoined:
		// �S�Ẵv���C���[�����[���ɃW���C������̂�҂��Ă���B
		break;
	case enStep_WaitAllPlayerStartGame:
		// �S�Ẵv���C���[���Q�[���J�n�\�ɂȂ�̂�҂��Ă���B
		break;
	case enStep_InGame: {
		// �C���Q�[��
		for (int i = 0; i < 2; i++) {
			const Vector3& actorPos = m_actor[i]->GetPosition();
			wchar_t text[256];
			swprintf_s(text, L"1P x : %f, y : %f, z : %f\n", actorPos.x, actorPos.y, actorPos.z);
			m_positionRender[i].SetText(text);
		}
	}break;
	case enStep_Error:
		delete m_onlineTwoPlayerMatchEngine;
		m_onlineTwoPlayerMatchEngine = nullptr;
		m_step = enStep_CharacterSelect;
		m_fontRender.SetText(L"A�{�^�� : �L����A�AB�{�^��: �L����B\n");
		DeleteGO(m_actor[0]);
		DeleteGO(m_actor[1]);
		m_charaNo = -1;
		
		break;
	}
}
void Game::OnAllPlayerJoined(void* pData, int size)
{
	// ���ׂẴv���C���[���������B
	m_actor[0] = NewGO<Actor>(0, "Actor");
	m_actor[1] = NewGO<Actor>(0, "Actor");
	const char* modelPath[] = {
		"Assets/modelData/human/human.tkm",
		"Assets/modelData/enemy/enemy.tkm",
	};
	const Vector3 pos[] = {
		{100.0f, 0.0f, 0.0f},		// 1P�̏������W
		{-100.0f, 0.0f, 0.0f},		// 2P�̏������W�B
	};
	Quaternion rot[2];
	rot[0].SetRotationDegY(-90.0f);
	rot[1].SetRotationDegY(90.0f);
	// �����̃v���C���[�ԍ����擾�B
	int playerNo = m_onlineTwoPlayerMatchEngine->GetPlayerNo();
	int otherPlayerNo = m_onlineTwoPlayerMatchEngine->GetOtherPlayerNo();
	// ����
	m_actor[playerNo]->Init(
		m_onlineTwoPlayerMatchEngine->GetGamePad(playerNo),
		modelPath[m_charaNo],
		pos[playerNo],
		rot[playerNo],
		m_actor[otherPlayerNo]
	);
	// �ΐ푊��
	m_actor[otherPlayerNo]->Init(
		m_onlineTwoPlayerMatchEngine->GetGamePad(otherPlayerNo),
		modelPath[*(int*)pData],
		pos[otherPlayerNo],
		rot[otherPlayerNo],
		m_actor[playerNo]
	);

	
	// ���[�h���I����ăQ�[���J�n�\�ɂȂ������Ƃ�ʒm����B
	m_onlineTwoPlayerMatchEngine->NotifyPossibleStartPlayGame();
	// �ق��̃v���C���[���Q�[���J�n�\�ɂȂ�܂ő҂B
	m_step = enStep_WaitAllPlayerStartGame;

	m_fontRender.SetText(L"WaitAllPlayerStartGame!!");
}

void Game::OnAllPlayerStartGame()
{
	// ���ׂẴv���C���[���Q�[���J�n�\�ɂȂ����̂ŁA�Q�[���X�^�[�g�I
	m_fontRender.SetText(L"InGame!!");
	m_step = enStep_InGame;

}
void Game::OnError()
{
	MessageBoxA(nullptr, "�ʐM�G���[���N���܂����B", "�G���[", MB_OK);
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