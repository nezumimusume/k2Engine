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

	// �o�́B
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
// �E�B���h�E�v���O�����̃��C���֐��B
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//�Q�[���̏������B
	InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));


	InitStandardIOConsole();

	//////////////////////////////////////
	// �������珉�������s���R�[�h���L�q����B
	//////////////////////////////////////


	//Game�N���X�̃I�u�W�F�N�g���쐬�B
	NewGO<Game>(0, "game");


	//////////////////////////////////////
	// ���������s���R�[�h�������̂͂����܂ŁI�I�I
	//////////////////////////////////////
	
	Stopwatch sw;
	// ��������Q�[�����[�v�B
	while (DispatchWindowMessage())
	{
		sw.Start();
		K2Engine::GetInstance()->Execute();

		sw.Stop();
		printf("fps = %0.2f\n", 1.0f / sw.GetElapsed());
	}


	CloseStandardIOConsole();

	// k2Engine�̏I�������B
	K2Engine::DeleteInstance();

#ifdef _DEBUG
	ReportLiveObjects();
#endif // _DEBUG
	return 0;
}

