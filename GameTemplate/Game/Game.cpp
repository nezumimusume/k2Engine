#include "stdafx.h"
#include "Game.h"


bool Game::Start()
{
	g_renderingEngine->DisableRaytracing();
	g_renderingEngine->SetDirectionLight(0, { 0.4f, -1.0f, 0.4f }, { 1.0f, 1.0f, 1.0f });
	g_renderingEngine->SetCascadeNearAreaRates( 0.05f, 0.3f, 1.0f );
	m_modelRender.Init("Assets/modelData/unityChan.tkm");
	m_bgRender.Init("Assets/modelData/bg.tkm");
	m_groundRender.Init("Assets/modelData/ground.tkm", nullptr, 0, enModelUpAxisZ, true, 1, false);
	
	g_camera3D->SetPosition({ 0.0f, 100.0f, 0.0f });
	g_camera3D->SetTarget({ 0.0f, 100.0f, 100.0f });
	g_camera3D->SetFar(50000.0f);
	return true;
}

void Game::Update()
{
	m_groundRender.Update();
	m_bgRender.Update();
	m_modelRender.Update();
}

void Game::Render(RenderContext& rc)
{
	g_camera3D->MoveForward(g_pad[0]->GetLStickYF() * 10.0f);
	g_camera3D->RotateY(g_pad[0]->GetLStickXF() * 0.03f);
	g_camera3D->MoveUp(g_pad[0]->GetRStickYF() * 10.0f);
	m_groundRender.Draw(rc);
	m_bgRender.Draw(rc);
	m_modelRender.Draw(rc);
}