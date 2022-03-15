#include "stdafx.h"
#include "system/system.h"

#include<InitGUID.h>
#include<dxgidebug.h>

#include "graphics/RenderingEngine.h"
#include "sound/SoundEngine.h"

#include "Game.h"


#include <io.h>
#include <Fcntl.h>
#include <iostream>


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

int hConsole = 0;

void InitStandardIOConsole()
{

	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);

}

void CloseStandardIOConsole()
{
	FreeConsole();
}

///////////////////////////////////////////////////////////////////
// ウィンドウプログラムのメイン関数。
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//ゲームの初期化。
	InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));


	InitStandardIOConsole();

	//////////////////////////////////////
	// ここから初期化を行うコードを記述する。
	//////////////////////////////////////


	//Gameクラスのオブジェクトを作成。
	NewGO<Game>(0, "game");


	//////////////////////////////////////
	// 初期化を行うコードを書くのはここまで！！！
	//////////////////////////////////////
	
	Stopwatch sw;
	// ここからゲームループ。
	while (DispatchWindowMessage())
	{
		sw.Start();
		K2Engine::GetInstance()->Execute();

		sw.Stop();
		printf("fps = %0.2f\n", 1.0f / sw.GetElapsed());
	}


	CloseStandardIOConsole();

	// k2Engineの終了処理。
	K2Engine::DeleteInstance();

#ifdef _DEBUG
	ReportLiveObjects();
#endif // _DEBUG
	return 0;
}

