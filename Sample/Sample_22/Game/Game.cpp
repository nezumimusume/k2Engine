#include "stdafx.h"
#include "Game.h"

#include "Player.h"
#include "GameCamera.h"
#include "Sphere.h"
#include "SphereRender.h"
#include "Muscle.h"
#include "MuscleRender.h"

Game::Game()
{
	//削除。
	DeleteGO(m_player);
	DeleteGO(m_gameCamera);
	for (auto sphere : m_spheres)
	{
		DeleteGO(sphere);
	}
	DeleteGO(m_sphereRender);
	for (auto muscle : m_muscles)
	{
		DeleteGO(muscle);
	}
	DeleteGO(m_muscleRender);
}

Game::~Game()
{

}

bool Game::Start()
{

	g_camera3D->SetPosition({ 0.0f, 100.0f, -600.0f });


	m_gameCamera = NewGO<GameCamera>(0, "gameCamera");

	//レベルを構築する。
	m_levelRender.Init("Assets/level/sample.tkl", [&](LevelObjectData& objData) {
		if (objData.ForwardMatchName(L"sample1") == true) {
			
			auto sphere = NewGO<Sphere>(0);
			//配置座標、スケール、回転を取得する。
			sphere->m_position = objData.position;
			sphere->m_scale = objData.scale;
			sphere->m_rotation = objData.rotation;
			sphere->m_instanceNo = m_maxSphere;
			//球の数を数える。
			m_maxSphere++;
			m_spheres.push_back(sphere);
			//trueにすると、レベルの方でモデルが読み込まれない。
			return true;
		}
		else if (objData.ForwardMatchName(L"sample2") == true) {

			auto muscle = NewGO<Muscle>(0);
			//配置座標、スケール、回転を取得する。
			muscle->m_position = objData.position;
			muscle->m_scale = objData.scale;
			muscle->m_rotation = objData.rotation;
			muscle->m_instanceNo = m_maxMuscle;
			//球の数を数える。
			m_maxMuscle++;
			m_muscles.push_back(muscle);
			
			//trueにすると、レベルの方でモデルが読み込まれない。
			return true;
		}
		else if (objData.EqualObjectName(L"sample3") == true) {
			//falseにすると、レベルの方でモデルが読み込まれて配置される。
			//レベルのモデルもインスタンシング描画に対応。
			return true;
		}
		else if (objData.EqualObjectName(L"player") == true) {
			//Unityちゃん。
			//プレイヤーのインスタンスを生成する。
			m_player = NewGO<Player>(0, "player");
			m_player->m_position = objData.position;
			m_player->m_scale = objData.scale;
			//trueにすると、レベルの方でモデルが読み込まれない。
			return true;
		}
		if (objData.EqualObjectName(L"background") == true) {
			//falseにすると、レベルの方でモデルが読み込まれて配置される。
			return false;
		}
		return true;
		});

	m_sphereRender = NewGO<SphereRender>(0,"sphererender");
	m_sphereRender->SetMaxSphere(m_maxSphere);

	m_muscleRender = NewGO<MuscleRender>(0, "musclerender");
	m_muscleRender->SetMaxMuscle(m_maxMuscle);

	return true;
}

void Game::Update()
{
	//レベルの更新処理。
	m_levelRender.Update();

	if (g_pad[0]->IsTrigger(enButtonA))
	{
		if (m_isDrawInstancing == true)
		{
			m_isDrawInstancing = false;
		}
		else
		{
			m_isDrawInstancing = true;
		}
	}

	
}

void Game::Render(RenderContext& rc)
{
	//レベルで読み込んだモデルを表示させる。
	m_levelRender.Draw(rc);
}
