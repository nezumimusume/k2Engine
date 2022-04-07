#include "stdafx.h"
#include "Game.h"

#include "Player.h"
#include "GameCamera.h"
#include "Box.h"

Game::Game()
{
	
}

Game::~Game()
{
	//削除。
	DeleteGO(m_player);
	DeleteGO(m_gameCamera);
	for (auto box : m_boxs)
	{
		DeleteGO(box);
	}
	for (auto pointLight : m_pointLightList)
	{
		//ポイントライトを削除する。
		delete pointLight;
	}
}

bool Game::Start()
{
	g_camera3D->SetPosition({ 0.0f, 100.0f, 800.0f });


	m_gameCamera = NewGO<GameCamera>(0, "gameCamera");

	//レベルを構築する。
	m_levelRender.Init("Assets/level/sample.tkl", [&](LevelObjectData& objData) {
		if (objData.EqualObjectName(L"sample") == true) {

			auto box = NewGO<Box>(0);
			//配置座標、スケール、回転を取得する。
			box->m_position = objData.position;
			box->m_scale = objData.scale;
			box->m_rotation = objData.rotation;
			
			//ポイントライトを作成する。
			PointLight* pointLight = new PointLight;
			pointLight->Init();
			Vector3 pointLightPosition = objData.position;
			pointLightPosition.y += 100.0f;
			//ポイントライトの座標を設定する。
			pointLight->SetPosition(pointLightPosition);
			m_pointLightList.push_back(pointLight);
			//trueにすると、レベルの方でモデルが読み込まれない。
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
		if (objData.EqualObjectName(L"bg") == true) {
			//falseにすると、レベルの方でモデルが読み込まれて配置される。
			return false;
		}
		return false;
	});


	//ポイントライトのパラメータを設定する。
	//ポイントライトの色。
	m_pointLightList[0]->SetColor(Vector3(40.0f, 5.0f, 5.0f));
	//ポイントライトの影響範囲。
	m_pointLightList[0]->SetRange(250.0f);
	//ポイントライトの減衰率。
	m_pointLightList[0]->SetAffectPowParam(1.0f);

	m_pointLightList[1]->SetColor(Vector3(5.0f, 20.0f, 5.0f));
	m_pointLightList[1]->SetRange(350.0f);
	m_pointLightList[1]->SetAffectPowParam(3.0f);

	m_pointLightList[2]->SetColor(Vector3(5.0f, 5.0f, 20.0f));
	m_pointLightList[2]->SetRange(500.0f);
	m_pointLightList[2]->SetAffectPowParam(2.0f);
	return true;
}

void Game::Update()
{
	for (auto pt : m_pointLightList) {
		pt->Update();
	}
}

void Game::Render(RenderContext& rc)
{
	//レベルで読み込んだモデルを表示させる。
	m_levelRender.Draw(rc);
}
