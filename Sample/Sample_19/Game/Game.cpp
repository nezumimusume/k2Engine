#include "stdafx.h"
#include "Game.h"

#include "Player.h"
#include "Background.h"
#include "GameCamera.h"
#include "Box.h"

Game::Game()
{
	//削除。
	DeleteGO(m_player);
	DeleteGO(m_background);
	DeleteGO(m_gameCamera);
	DeleteGO(m_box);
}

Game::~Game()
{

}

bool Game::Start()
{
	g_camera3D->SetPosition({ 0.0f, 100.0f, -600.0f });

	m_player = NewGO<Player>(0, "player");
	m_background = NewGO<Background>(0, "background");
	m_gameCamera = NewGO<GameCamera>(0, "gameCamera");
	m_box = NewGO<Box>(0, "box");

	//当たり判定有効化。
	PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();

	m_fontRender.SetText(L"衝突！！");
	m_fontRender.SetScale(2.0f);
	m_fontRender.SetPosition(Vector3(100.0f, 100.0f, 0.0f));
	return true;
}

void Game::Update()
{
	m_isHit = false;
	//キャラクタコントローラーとゴーストオブジェクトのあたり判定を行う。
	PhysicsWorld::GetInstance()->ContactTest(m_player->m_charaCon, [&](const btCollisionObject& contactObject) {
		if (m_box->m_physicsGhostObject.IsSelf(contactObject) == true) {
			//m_physicsGhostObjectとぶつかった。
			//フラグをtrueにする。
			m_isHit = true;
		}
	});
}

void Game::Render(RenderContext& rc)
{
	//キャラクターコントローラーとゴーストオブジェクトが衝突していなかったら。
	if (m_isHit == false)
	{
		//何もしない。
		return;
	}

	//文字表示。
	m_fontRender.Draw(rc);
}