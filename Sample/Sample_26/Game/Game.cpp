#include "stdafx.h"
#include "Game.h"

#include "Player.h"
#include "BackGround.h"
#include "GameCamera.h"
#include "Lantern.h"


bool Game::Start()
{
	m_gameCamera = NewGO<GameCamera>(0);

	//レベルを構築する。
	m_levelRender.Init("Assets/level/sample.tkl", [&](LevelObjectData& objData) {
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
		else if (objData.ForwardMatchName(L"Lantern") == true)
		{
			auto lantern = NewGO<Lantern>(0);
			lantern->SetPosition(objData.position);
			lantern->SetRotation(objData.rotation);
			lantern->SetScale(objData.scale);
			m_lanternList.push_back(lantern);
			return true;
		}
		return true;
		});
	return true;
}

void Game::Update()
{

}

void Game::Render(RenderContext& rc)
{
	
}