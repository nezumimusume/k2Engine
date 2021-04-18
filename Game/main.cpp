#include "stdafx.h"
#include "system/system.h"
#include "Player.h"
#include "sound/SoundEngine.h"
#include "graphics/RenderingEngine.h"

///////////////////////////////////////////////////////////////////
// ウィンドウプログラムのメイン関数。
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//ゲームの初期化。
	InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

	//////////////////////////////////////
	// ここから初期化を行うコードを記述する。
	//////////////////////////////////////

	//ゲームオブジェクトマネージャーのインスタンスを作成する。
	GameObjectManager::CreateInstance();
	PhysicsWorld::CreateInstance();
	g_soundEngine = new SoundEngine();
	//レンダリングエンジンを初期化
	g_renderingEngine = new RenderingEngine();
	g_renderingEngine->Init();

	NewGO<Player>(0,"player");
	Player* player = NewGO<Player>(0, "player");
	player->m_position = { 0.0f,-100.0f,0.0f };
	player->m_scale = { 5.0f,0.5f,5.0f };
	//NewGO<Player>(0,"player");
	//NewGO<Player>(0,"player");
	//NewGO<Player>(0,"player");
	//NewGO<Player>(0,"player");

	//////////////////////////////////////
	// 初期化を行うコードを書くのはここまで！！！
	//////////////////////////////////////
	auto& renderContext = g_graphicsEngine->GetRenderContext();

	// ここからゲームループ。
	while (DispatchWindowMessage())
	{
		//レンダリング開始。
		g_engine->BeginFrame();
		

		//////////////////////////////////////
		//ここから絵を描くコードを記述する。
		//////////////////////////////////////
		
		//sprite[1].Update(pos[1], Quaternion::Identity, Vector3::One);
		GameObjectManager::GetInstance()->ExecuteUpdate();
		GameObjectManager::GetInstance()->ExecuteRender(renderContext);

		//レンダリングエンジンを実行
		g_renderingEngine->Execute(renderContext);
		PhysicsWorld::GetInstance()->DebubDrawWorld(renderContext);
	
		//////////////////////////////////////
		//絵を描くコードを書くのはここまで！！！
		//////////////////////////////////////
		g_soundEngine->Update();
		g_engine->EndFrame();
	}
	//ゲームオブジェクトマネージャーを削除。
	GameObjectManager::DeleteInstance();
	return 0;
}

