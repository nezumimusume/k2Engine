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
	// tkn�t�@�C�������[�h�B
	m_tknFile.Load("Assets/nvm/test.tkn");
	// �i�r���b�V�����\�z�B
	m_nvmMesh.Init(m_tknFile);
	return true;
}

void Game::Update()
{
	// �e�X�g�œK���ȃp�X�������s���B
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