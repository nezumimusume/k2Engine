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
	//.wavファイルを読み込む。
	//BGM。
	g_soundEngine->ResistWaveFileBank(0, "Assets/sound/shining_star.wav");
	//効果音。
	g_soundEngine->ResistWaveFileBank(1, "Assets/sound/get.wav");

	//SoundSourceのオブジェクトを作成する。
	//SoundSouceはIGameObjectを継承しているので。
	//NewGOする。
	m_soundSource = NewGO<SoundSource>(0);
	//ResistWaveFileBankで指定した番号。
	m_soundSource->Init(0);
	//BGMは曲をループさせる。
	m_soundSource->Play(true);

	return true;
}

void Game::Update()
{
	//Aボタンが押されたら。
	if (g_pad[0]->IsTrigger(enButtonA))
	{
		//BGMが再生中なら。
		if (m_soundSource->IsPlaying())
		{
			//停止させる。
			m_soundSource->Stop();
		}
		//停止中なら。
		else
		{
			m_soundSource->Play(true);
		}
	}

	//Bボタンが押されたら。
	if (g_pad[0]->IsTrigger(enButtonB))
	{
		//効果音を鳴らす。
		SoundSource* soundSource = NewGO<SoundSource>(0);
		soundSource->Init(1);
		//効果音はワンショット再生(ループさせない)なので。
		//false
		soundSource->Play(false);
	}

}