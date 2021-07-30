#include "stdafx.h"
#include "system/system.h"

#include<InitGUID.h>
#include<dxgidebug.h>

#include "graphics/RenderingEngine.h"
#include "sound/SoundEngine.h"
#include "collision/CollisionObject.h"

#include "Game.h"



void ReportLiveObjects()
{
	IDXGIDebug* pDxgiDebug;

	typedef HRESULT(__stdcall* fPtr)(const IID&, void**);
	HMODULE hDll = GetModuleHandleW(L"dxgidebug.dll");
	fPtr DXGIGetDebugInterface = (fPtr)GetProcAddress(hDll, "DXGIGetDebugInterface");

	DXGIGetDebugInterface(__uuidof(IDXGIDebug), (void**)&pDxgiDebug);

	// 出力。
	pDxgiDebug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_DETAIL);
}

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

	//各種エンジンなどの初期化。
	//ゲームオブジェクトマネージャーのインスタンスを作成する。
	GameObjectManager::CreateInstance();
	PhysicsWorld::CreateInstance();
	g_soundEngine = new SoundEngine();
	//レンダリングエンジンを初期化
	g_renderingEngine = new RenderingEngine();
	g_renderingEngine->Init();
	g_collisionObjectManager = new CollisionObjectManager();
	//エフェクトエンジンの初期化。
	EffectEngine::CreateInstance();



	//Gameクラスのオブジェクトを作成。
	NewGO<Game>(0, "game");
	
	


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

		GameObjectManager::GetInstance()->ExecuteUpdate();

		//エフェクトエンジンの更新。
		EffectEngine::GetInstance()->Update(g_gameTime->GetFrameDeltaTime());


		GameObjectManager::GetInstance()->ExecuteRender(renderContext);
		//レンダリングエンジンを実行。
		//ここでエフェクトをドロー。
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

	PhysicsWorld::DeleteInstance();
	delete g_soundEngine;
	EffectEngine::DeleteInstance();
	delete g_renderingEngine;

#ifdef _DEBUG
	ReportLiveObjects();
#endif // _DEBUG
	return 0;
}

