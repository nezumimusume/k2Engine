#include "stdafx.h"
#include "Game.h"


Game::Game()
{

}

Game::~Game()
{
	g_sceneLight->DeletePointLight(m_targetPointPointLight);
}

bool Game::Start()
{
	g_camera3D->SetViewAngle(Math::DegToRad(90.0f));
	g_camera3D->SetPosition(0.0f, 700.0f, 300.0f);
	
	g_camera3D->SetTarget(0.0f, 400.0f, 0.0f);
	g_camera3D->SetFar(10000.0f);
	m_bgModelRendedr.Init("Assets/modelData/bg/bg.tkm");
	m_bgObject.CreateFromModel(m_bgModelRendedr.GetModel(), m_bgModelRendedr.GetWorldMatrix(0));

	m_charaRender.Init("Assets/modelData/unityChan.tkm");
	m_charaRender.SetScale(1.2f, 1.2f, 1.2f);
	m_targetPointRender.Init("Assets/modelData/light.tkm");
	m_targetPointRender.SetScale(4.0f, 4.0f, 4.0f);
	m_charaCon.Init(50.0f, 50.0f, m_targetPointPosition);
	m_targetPointPointLight = g_sceneLight->NewPointLight();
	
	// �i�r���b�V�����\�z�B
	m_nvmMesh.Init("Assets/nvm/test.tkn");
	
	SkyCube* sky = NewGO<SkyCube>(0);
	sky->SetLuminance(0.2f);
	return true;
}

void Game::Update()
{
	bool isEnd;
	if (g_pad[0]->IsTrigger(enButtonA)) {
		// �p�X����
		m_pathFiding.Execute(
			m_path,							// �\�z���ꂽ�p�X�̊i�[��
			m_nvmMesh,						// �i�r���b�V��
			m_position,						// �J�n���W
			m_targetPointPosition,			// �ړ��ڕW���W
			PhysicsWorld::GetInstance(),	// �����G���W��	
			50.0f,							// AI�G�[�W�F���g�̔��a
			200.0f							// AI�G�[�W�F���g�̍����B
		);
	}
	// �p�X����ړ�����B
	m_position = m_path.Move(
		m_position, 
		10.0f, 
		isEnd
	);
	m_charaRender.SetPosition(m_position);
	m_charaRender.Update();

	Vector3 move;
	move.x = -g_pad[0]->GetLStickXF() * 10.0f;
	move.y = -10.0f;
	move.z = -g_pad[0]->GetLStickYF() * 10.0f;
	m_targetPointPosition = m_charaCon.Execute(move, 1.0f);
	m_targetPointRender.SetPosition(
		{ m_targetPointPosition.x, m_targetPointPosition.y + 100.0f, m_targetPointPosition.z }
	);
	m_targetPointPointLight->SetPosition(
		{ m_targetPointPosition.x, m_targetPointPosition.y + 100.0f, m_targetPointPosition.z }
	);
	m_targetPointPointLight->SetColor({ 0.5f, 0.5f, 0.5f });
	m_targetPointPointLight->SetRange(400.0f);
	m_targetPointPointLight->Update();

	m_targetPointRender.Update();
}

void Game::Render(RenderContext& rc)
{
	m_bgModelRendedr.Draw(rc);
	m_charaRender.Draw(rc);
	m_targetPointRender.Draw(rc);
}