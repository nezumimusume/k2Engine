#include "stdafx.h"
#include "Game.h"
#include "Player.h"

bool Game::Start()
{

	//レベルを構築する。
	m_levelRender.Init("Assets/level/sample.tkl", [&](LevelObjectData& objData) {
		if (objData.EqualObjectName(L"unityChan") == true) {

			return true;
		}
		else if (objData.EqualObjectName(L"player") == true) {
			//Unityちゃん。
			//プレイヤーのインスタンスを生成する。
			m_player = NewGO<Player>(0, "player");
			m_player->position = objData.position;
			//フックした場合はtrueを返す。
			return true;
		}
		return false;
		});
	return true;
}

void Game::Render(RenderContext& rc)
{
	m_levelRender.Draw(rc);
}