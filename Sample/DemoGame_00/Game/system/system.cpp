#include "stdafx.h"
#include "system.h"
#include "graphics/GraphicsEngine.h"
#include "graphics/RenderingEngine.h"
#include "sound/SoundEngine.h"

HWND			g_hWnd = NULL;				//ウィンドウハンドル。

///////////////////////////////////////////////////////////////////
//メッセージプロシージャ。
//hWndがメッセージを送ってきたウィンドウのハンドル。
//msgがメッセージの種類。
//wParamとlParamは引数。今は気にしなくてよい。
///////////////////////////////////////////////////////////////////
LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//送られてきたメッセージで処理を分岐させる。
	switch (msg)
	{
	case WM_DESTROY:
		//スエンジンの破棄。
		PostQuitMessage(0);
		break;	
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

///////////////////////////////////////////////////////////////////
// ウィンドウの初期化。
///////////////////////////////////////////////////////////////////
void InitWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow, const TCHAR* appName)
{
	//ウィンドウクラスのパラメータを設定(単なる構造体の変数の初期化です。)
	WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX),		//構造体のサイズ。
		CS_CLASSDC,				//ウィンドウのスタイル。
								//ここの指定でスクロールバーをつけたりできるが、ゲームでは不要なのでCS_CLASSDCでよい。
		MsgProc,				//メッセージプロシージャ(後述)
		0,						//0でいい。
		0,						//0でいい。
		GetModuleHandle(NULL),	//このクラスのためのウインドウプロシージャがあるインスタンスハンドル。
								//何も気にしなくてよい。
		NULL,					//アイコンのハンドル。アイコンを変えたい場合ここを変更する。とりあえずこれでいい。
		NULL,					//マウスカーソルのハンドル。NULLの場合はデフォルト。
		NULL,					//ウィンドウの背景色。NULLの場合はデフォルト。
		NULL,					//メニュー名。NULLでいい。
		appName,				//ウィンドウクラスに付ける名前。
		NULL					//NULLでいい。
	};
	//ウィンドウクラスの登録。
	RegisterClassEx(&wc);

	// ウィンドウの作成。
	g_hWnd = CreateWindow(
		appName,				//使用するウィンドウクラスの名前。
								//先ほど作成したウィンドウクラスと同じ名前にする。
		appName,				//ウィンドウの名前。ウィンドウクラスの名前と別名でもよい。
		WS_OVERLAPPEDWINDOW,	//ウィンドウスタイル。ゲームでは基本的にWS_OVERLAPPEDWINDOWでいい、
		0,						//ウィンドウの初期X座標。
		0,						//ウィンドウの初期Y座標。
		FRAME_BUFFER_W,			//ウィンドウの幅。
		FRAME_BUFFER_H,			//ウィンドウの高さ。
		NULL,					//親ウィンドウ。ゲームでは基本的にNULLでいい。
		NULL,					//メニュー。今はNULLでいい。
		hInstance,				//アプリケーションのインスタンス。
		NULL
	);

	ShowWindow(g_hWnd, nCmdShow);

}


//ゲームの初期化。
void InitGame(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow, const TCHAR* appName)
{
	//ウィンドウを初期化。
	InitWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow, appName);
	//k2エンジンの初期化。
	g_engine = new K2EngineLow;
	g_engine->Init(g_hWnd, FRAME_BUFFER_W, FRAME_BUFFER_H);
	g_camera3D->SetPosition({ 0.0f, 100.0f, -200.0f });
	g_camera3D->SetTarget({ 0.0f, 50.0f, 0.0f });
}
//ウィンドウメッセージをディスパッチ。falseが返ってきたら、ゲーム終了。
bool DispatchWindowMessage()
{
	MSG msg = { 0 };
	while (WM_QUIT != msg.message) {
		//ウィンドウからのメッセージを受け取る。
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			//ウィンドウメッセージが空になった。
			break;
		}
	}
	return msg.message != WM_QUIT;
}

void InitK2Engine(bool isSoftShadow)
{
	//各種エンジンなどの初期化。
	//ゲームオブジェクトマネージャーのインスタンスを作成する。
	GameObjectManager::CreateInstance();
	PhysicsWorld::CreateInstance();
	g_soundEngine = new SoundEngine();
	//レンダリングエンジンを初期化
	RenderingEngine::CreateInstance(isSoftShadow);
	//エフェクトエンジンの初期化。
	EffectEngine::CreateInstance();
}
void ExecuteK2Engine()
{
	auto& renderContext = g_graphicsEngine->GetRenderContext();

	//レンダリング開始。
	g_engine->BeginFrame();
	g_soundEngine->Update();

	//////////////////////////////////////
	//ここから絵を描くコードを記述する。
	//////////////////////////////////////

	GameObjectManager::GetInstance()->ExecuteUpdate();

	// エフェクトエンジンの更新。
	EffectEngine::GetInstance()->Update(g_gameTime->GetFrameDeltaTime());
	// レンダリングエンジンの更新。
	RenderingEngine::GetInstance()->Update();


	// ゲームオブジェクトマネージャーの描画処理を実行。
	GameObjectManager::GetInstance()->ExecuteRender(renderContext);
	//レンダリングエンジンを実行。		
	RenderingEngine::GetInstance()->Execute(renderContext);

	PhysicsWorld::GetInstance()->DebubDrawWorld(renderContext);

	//////////////////////////////////////
	//絵を描くコードを書くのはここまで！！！
	//////////////////////////////////////
	g_engine->EndFrame();
}
void FinalK2Engine()
{
	//ゲームオブジェクトマネージャーを削除。
	GameObjectManager::DeleteInstance();
	PhysicsWorld::DeleteInstance();
	EffectEngine::DeleteInstance();
	RenderingEngine::DeleteInstance();

	delete g_soundEngine;
	delete g_engine;
}