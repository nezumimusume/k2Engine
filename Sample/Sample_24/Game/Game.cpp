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
	m_fontRender.SetText(L"A�{�^�� : �L����A�AB�{�^��: �L����B\n");
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
			// �L�������I�΂ꂽ�̃I�����C���}�b�`���X�^�[�g����B
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
			// ���ׂẴv���C���[�������̂�҂B
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
		m_fontRender.SetText(L"A�{�^�� : �L����A�AB�{�^��: �L����B\n");
		break;
	}
}
void Game::OnAllPlayerJoined(void* pData, int size)
{
	// ���ׂẴv���C���[���������B
	m_modelRender->Init("Assets/modelData/unityChan.tkm");
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
		m_modelRender->Draw(rc);
		break;
	}
}