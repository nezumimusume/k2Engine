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
	g_camera3D->SetViewAngle(Math::DegToRad(90.0f));
	g_camera3D->SetPosition(0.0f, 1000.0f, -400.0f);
	
	g_camera3D->SetTarget(0.0f, 50.0f, -500.0f);
	g_camera3D->SetFar(10000.0f);
	m_bgModelRendedr.Init("Assets/modelData/bg/bg.tkm");
	m_bgObject.CreateFromModel(m_bgModelRendedr.GetModel(), m_bgModelRendedr.GetWorldMatrix(0));

	m_charaRender.Init("Assets/modelData/unityChan.tkm");
	m_charaRender.SetScale(1.2f, 1.2f, 1.2f);
	m_targetPointRender.Init("Assets/modelData/light.tkm");
	m_targetPointRender.SetScale(4.0f, 4.0f, 4.0f);

	// tknファイルをロード。
	m_tknFile.Load("Assets/nvm/test.tkn");
	// ナビメッシュを構築。
	m_nvmMesh.Init(m_tknFile);
	// テストで適当なパス検索を行う。
	// 後でBSPのテストをするのでいったんコメントアウト。
	m_pathFiding.Execute(
		m_path,
		m_nvmMesh,
		{ 0.0f, 0.0f, 0.0f },
		{ 200.0f, 0.0f, 0.0f },
		PhysicsWorld::GetInstance(),
		50.0f,
		200.0f
	);
	return true;
}

void Game::Update()
{
	bool isEnd;
	if (g_pad[0]->IsTrigger(enButtonA)) {
		// パス検索
		m_pathFiding.Execute(
			m_path,
			m_nvmMesh,
			m_position,
			m_targetPointPosition,
			PhysicsWorld::GetInstance(),
			50.0f,
			200.0f
		);
		m_path.StartWalkPath();

	}
	m_position = m_path.WalkPath(m_position, 10.0f, isEnd);
	m_charaRender.SetPosition(m_position);
	m_charaRender.Update();

	m_targetPointPosition.x -= g_pad[0]->GetLStickXF() * 10.0f;
	m_targetPointPosition.z -= g_pad[0]->GetLStickYF() * 10.0f;
	m_targetPointRender.SetPosition(m_targetPointPosition);
	m_targetPointRender.Update();
}

void Game::Render(RenderContext& rc)
{
	m_bgModelRendedr.Draw(rc);
	m_charaRender.Draw(rc);
	m_targetPointRender.Draw(rc);
}