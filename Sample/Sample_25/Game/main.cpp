#include "stdafx.h"
#include "system/system.h"

#include<InitGUID.h>
#include<dxgidebug.h>

#include "graphics/RenderingEngine.h"
#include "sound/SoundEngine.h"

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

	BSP bsp;
	bsp.AddLeaf( { 183.333344,0.00000000,   16.6666679		}, nullptr);
	bsp.AddLeaf( { 166.666672,0.00000000,   -33.3333359		}, nullptr);
	bsp.AddLeaf( { 133.333344, 0.00000000,  -66.6666718		}, nullptr);
	bsp.AddLeaf( { -266.666687, 0.00000000, 416.666687		}, nullptr);
	bsp.AddLeaf( { -283.333344, 0.00000000, 433.333344		}, nullptr);
	bsp.Build();
	bsp.WalkTree({ 200.0, 0.0,0.0 }, [&](BSP::SLeaf* leaf) {
		int hoge = 0;
	});
	//////////////////////////////////////
	// ここから初期化を行うコードを記述する。
	//////////////////////////////////////

	//Gameクラスのオブジェクトを作成。
	NewGO<Game>(0, "game");
	
	//////////////////////////////////////
	// 初期化を行うコードを書くのはここまで！！！
	//////////////////////////////////////
	auto& renderContext = g_graphicsEngine->GetRenderContext();

	// ここからゲームループ。
	while (DispatchWindowMessage())
	{
		K2Engine::GetInstance()->Execute();
	}
	K2Engine::DeleteInstance();

#ifdef _DEBUG
	ReportLiveObjects();
#endif // _DEBUG
	
	return 0;
}

