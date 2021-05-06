#include "stdafx.h"
#include "system/system.h"

#include<InitGUID.h>
#include<dxgidebug.h>


#include "Player.h"
#include "sound/SoundEngine.h"
#include "graphics/RenderingEngine.h"
#include "BackGround.h"
#include "GameCamera.h"
#include "effect/effect.h"

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

	//ゲームオブジェクトマネージャーのインスタンスを作成する。
	GameObjectManager::CreateInstance();
	PhysicsWorld::CreateInstance();
	//g_soundEngine = new SoundEngine();

	
	//レンダリングエンジンを初期化
	//g_renderingEngine = new RenderingEngine();
	//g_renderingEngine->Init();

	//NewGO<Player>(0,"player");
	//Player* player = NewGO<Player>(0, "player");
	//player->m_position = { 100.0f,00.0f,600.0f };
	//player->m_scale = { 4.0f,4.0f,4.0f };
	//NewGO<BackGround>(0, "backGround");
	//NewGO<GameCamera>(0, "gameCamera");

	//NewGO<Player>(0,"player");
	//NewGO<Player>(0,"player");
	//NewGO<Player>(0,"player");
	//NewGO<Player>(0,"player");

	//エフェクトエンジンの初期化。
	/*EffectEngine::CreateInstance();
	//step-2 レーザーエフェクトの初期化。
	Effect laserEffect;
	laserEffect.Init(u"Assets/effect/laser.efk");
	*/




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



		/*if (g_pad[0]->IsTrigger(enButtonA)) {
			//再生開始。
			laserEffect.Play();
		}
		auto pos = laserEffect.GetPosition();
		pos.x += g_pad[0]->GetLStickXF();
		pos.z += g_pad[0]->GetLStickYF();

		auto rot = laserEffect.GetRotation();
		rot.AddRotationY(g_pad[0]->GetRStickXF() * 0.1f);

		laserEffect.SetPosition(pos);
		laserEffect.SetRotation(rot);
		laserEffect.SetScale({ 10.0f,10.0f,10.0f });
		laserEffect.Update();
		//エフェクトエンジンの更新。
		EffectEngine::GetInstance()->Update(g_gameTime->GetFrameDeltaTime());
		//エフェクトのドロー。
		EffectEngine::GetInstance()->Draw();
		*/




		GameObjectManager::GetInstance()->ExecuteRender(renderContext);
	
		//レンダリングエンジンを実行
		//g_renderingEngine->Execute(renderContext);
		//PhysicsWorld::GetInstance()->DebubDrawWorld(renderContext);
		
	
		//////////////////////////////////////
		//絵を描くコードを書くのはここまで！！！
		//////////////////////////////////////
		//g_soundEngine->Update();
		g_engine->EndFrame();
	
	}
	//ゲームオブジェクトマネージャーを削除。
	GameObjectManager::DeleteInstance();

	//ReportLiveObjects();
	return 0;
}

