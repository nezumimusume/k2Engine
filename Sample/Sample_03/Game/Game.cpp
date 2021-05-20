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
	//アニメーションクリップのロード。
	m_animClips[enAnimationClip_idle].Load("Assets/animData/idle.tka");
	m_animClips[enAnimationClip_run].Load("Assets/animData/run.tka");
	m_animClips[enAnimationClip_walk].Load("Assets/animData/walk.tka");
	//ループフラグを設定する。<-走りアニメーションはループフラグを設定していないので
	//ワンショット再生で停止する。
	m_animClips[enAnimationClip_run].SetLoopFlag(true);
	m_animClips[enAnimationClip_idle].SetLoopFlag(true);
	m_animClips[enAnimationClip_walk].SetLoopFlag(true);


	//モデルを読み込む。
	m_modelRender.Init("Assets/modelData/unityChan.tkm",m_animClips,enAnimationClip_Num,enModelUpAxisY);

	return true;
}

void Game::Update()
{
	//Aボタンが押されたら待機モーションを再生する。
	if (g_pad[0]->IsTrigger(enButtonA)) {
		//Play関数の第二引数は補完時間。
		//待機モーションの切り替えは補完時間を設定していないので、アニメーションがパキっと変わる。
		m_modelRender.PlayAnimation(enAnimationClip_idle);
	}
	//Bボタンが押されたら走りアニメーションを再生する。
	else if (g_pad[0]->IsTrigger(enButtonB)) {
		m_modelRender.PlayAnimation(enAnimationClip_run, 0.2f);
	}
	//Yボタンが押されたら歩きアニメーションを再生する。
	else if (g_pad[0]->IsTrigger(enButtonY)) {
		m_modelRender.PlayAnimation(enAnimationClip_walk, 0.2f);
	}

	//モデルの更新。
	m_modelRender.Update();
}

void Game::Render(RenderContext& rc)
{
	//モデルの描画。
	m_modelRender.Draw(rc);
}