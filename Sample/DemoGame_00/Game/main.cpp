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

	// �o�́B
	pDxgiDebug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_DETAIL);
}


///////////////////////////////////////////////////////////////////
// �E�B���h�E�v���O�����̃��C���֐��B
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//�Q�[���̏������B
	InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));
	
	//////////////////////////////////////
	// �������珉�������s���R�[�h���L�q����B
	//////////////////////////////////////
	
	//Title�N���X�̃I�u�W�F�N�g���쐬�B
	NewGO<Title>(0, "title");
	//NewGO<Game>(0, "game");
	//Fade�N���X�̃I�u�W�F�N�g���쐬�B
	NewGO<Fade>(2, "fade");

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
		char text[256];
		sprintf(text, "time = %f\n", sw.GetElapsedMillisecond());
		OutputDebugStringA(text);
	}
	
	// k2Engine�̏I�������B
	K2Engine::DeleteInstance();

#ifdef _DEBUG
	ReportLiveObjects();
#endif // _DEBUG
	return 0;
}

