#include "stdafx.h"
#include "Game.h"

//EffectEmitterを使用する時はファイルをインクルードする必要がある。
#include "graphics/effect/EffectEmitter.h"

Game::Game()
{

}

Game::~Game()
{
	
}

bool Game::Start()
{
	//エフェクトを読み込む。
	EffectEngine::GetInstance()->ResistEffect(0, u"Assets/effect/laser.efk");
	EffectEngine::GetInstance()->ResistEffect(1, u"Assets/effect/laser2.efk");

	return true;
}

void Game::Update()
{
	//Aボタンが押されたら。
	if (g_pad[0]->IsTrigger(enButtonA))
	{
		EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
		effectEmitter->Init(0);
		effectEmitter->SetScale({ 5.0f,5.0f,5.0f });
		effectEmitter->Play();
	}

	//Bボタンが押されたら。
	if (g_pad[0]->IsTrigger(enButtonB))
	{
		EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
		effectEmitter->Init(1);
		effectEmitter->SetScale({ 5.0f,5.0f,5.0f });
		effectEmitter->Play();
	}

}