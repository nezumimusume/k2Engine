#include "stdafx.h"
#include "Game.h"


Game::Game()
{

}

Game::~Game()
{
	DeleteGO(m_soundSource);
}

bool Game::Start()
{
	//.wav�t�@�C����ǂݍ��ށB
	//BGM�B
	g_soundEngine->ResistWaveFileBank(0, "Assets/sound/shining_star.wav");
	//���ʉ��B
	g_soundEngine->ResistWaveFileBank(1, "Assets/sound/get.wav");

	//SoundSource�̃I�u�W�F�N�g���쐬����B
	//SoundSouce��IGameObject���p�����Ă���̂ŁB
	//NewGO����B
	m_soundSource = NewGO<SoundSource>(0);
	//ResistWaveFileBank�Ŏw�肵���ԍ��B
	m_soundSource->Init(0);
	//BGM�͋Ȃ����[�v������B
	m_soundSource->Play(true);

	return true;
}

void Game::Update()
{
	//A�{�^���������ꂽ��B
	if (g_pad[0]->IsTrigger(enButtonA))
	{
		//BGM���Đ����Ȃ�B
		if (m_soundSource->IsPlaying())
		{
			//��~������B
			m_soundSource->Stop();
		}
		//��~���Ȃ�B
		else
		{
			m_soundSource->Play(true);
		}
	}

	//B�{�^���������ꂽ��B
	if (g_pad[0]->IsTrigger(enButtonB))
	{
		//���ʉ���炷�B
		SoundSource* soundSource = NewGO<SoundSource>(0);
		soundSource->Init(1);
		//���ʉ��̓����V���b�g�Đ�(���[�v�����Ȃ�)�Ȃ̂ŁB
		//false
		soundSource->Play(false);
	}

}