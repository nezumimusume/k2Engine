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
	g_camera3D->SetPosition(0.0f, 500.0f, 500.0f);
	g_camera3D->SetTarget(0.0f, 50.0f, 0.0f);
	g_camera3D->SetFar(10000.0f);
	m_bgModelRendedr.Init("Assets/modelData/bg/bg.tkm");
	m_charaRender.Init("Assets/modelData/unityChan.tkm");
	// tknファイルをロード。
	m_tknFile.Load("Assets/nvm/test.tkn");
	// ナビメッシュを構築。
	m_nvmMesh.Init(m_tknFile);
	return true;
}

void Game::Update()
{
	// テストで適当なパス検索を行う。
	m_pathFiding.Execute(
		m_path,
		m_nvmMesh,
		{ 0.0f, 0.0f, 0.0f },
		{ 200.0f, 0.0f, 0.0f }
	);
}

void Game::Render(RenderContext& rc)
{
	m_bgModelRendedr.Draw(rc);
	m_charaRender.Draw(rc);
}