#include "stdafx.h"
#include "system/system.h"
#include<InitGUID.h>
#include<dxgidebug.h>
#include "Title.h"
#include "Game.h"

#include "Fade.h"




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
	// k2Engineの初期化
	InitK2Engine(true);

	//////////////////////////////////////
	// ここから初期化を行うコードを記述する。
	//////////////////////////////////////
	
	//Titleクラスのオブジェクトを作成。
	NewGO<Title>(0, "title");
	//NewGO<Game>(0, "game");
	//Fadeクラスのオブジェクトを作成。
	NewGO<Fade>(2, "fade");

	//////////////////////////////////////
	// 初期化を行うコードを書くのはここまで！！！
	//////////////////////////////////////
	
	Stopwatch sw;
	// ここからゲームループ。
	while (DispatchWindowMessage())
	{
		sw.Start();

		// k2Engineの処理を実行。
		ExecuteK2Engine();

		sw.Stop();
		char text[256];
		sprintf(text, "time = %f\n", sw.GetElapsedMillisecond());
		OutputDebugStringA(text);
	}
	
	FinalK2Engine();

#ifdef _DEBUG
	ReportLiveObjects();
#endif // _DEBUG
	return 0;
}

