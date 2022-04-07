#include "stdafx.h"
#include "Game.h"

#include "Player.h"
#include "BackGround.h"
#include "GameCamera.h"
#include "Dancer.h"
#include "GlowingObject.h"

Game::Game()
{

}

Game::~Game()
{
	DeleteGO(m_bgm);
}

bool Game::Start()
{
	
	//レベルを構築する。
	m_level.Init("Assets/level/sample.tkl", [&](LevelObjectData& objData) {
		if (objData.EqualObjectName(L"unityChan") == true) 
		{

			m_player = NewGO<Player>(0, "player");
			m_player->SetPosition(objData.position);
			m_player->SetRotation(objData.rotation);
			//trueにすると、レベルの方でモデルが読み込まれない。
			return true;
		}
		else if (objData.EqualObjectName(L"stage") == true) 
		{
			m_backGround = NewGO<BackGround>(0);
			m_backGround->SetPosition(objData.position);
			m_backGround->SetScale(objData.scale);
			return true;
		}
		else if (objData.EqualObjectName(L"maria") == true) 
		{
			m_dancer = NewGO<Dancer>(0, "Dancer");
			m_dancer->SetPosition(objData.position);
			m_dancer->SetScale(objData.scale);
			m_dancer->SetRotation(objData.rotation);
			return true;
		}
		else if (objData.EqualObjectName(L"light") == true)
		{
			
			for (int i = 0; i < m_glowingObjectList.size(); i++)
			{
				Vector3 diff = m_glowingObjectList[i]->GetPosition() - objData.position;
				if (diff.Length() < 1.0f)
				{
					return false;
				}
			}
			auto glowingObject = NewGO<GlowingObject>(0);
			glowingObject->SetPosition(objData.position);
			glowingObject->SetRotation(objData.rotation);
			m_glowingObjectList.push_back(glowingObject);
			return true;
		}
		return true;
		});
	m_gameCamera = NewGO<GameCamera>(0);

	g_soundEngine->ResistWaveFileBank(0, "Assets/sound/shiningstar.wav");
	g_renderingEngine->DisableTonemap();
	g_renderingEngine->SetSceneMiddleGray(0.05f);
	g_renderingEngine->SetAmbient({ 0.1f, 0.1f, 0.1f });
	g_renderingEngine->SetDirectionLight(1, { -1, 1, 0 }, { 3.0, 3.0, 3.0 });
	m_bgm = NewGO<SoundSource>(0);
	m_bgm->Init(0);
	m_bgm->Play(true);
	return true;
}

void Game::Update()
{
	
}

void Game::Render(RenderContext& rc)
{
	
}