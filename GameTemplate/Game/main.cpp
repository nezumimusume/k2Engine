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

	g_camera3D->SetPosition({ 0.0f, 0.0f, 100.0f });
	g_camera3D->SetTarget({ 0.0f, 0.0f, 0.0f });
	g_camera3D->SetFar(500000.0f);

	auto skyCube = NewGO<SkyCube>(0);
	skyCube->SetType(enSkyCubeType_DayToon_3);
	g_sceneLight->SetIBLTextureForAmbient(skyCube->GetTextureFilePath(), 0.1f);
	
	Vector3 ligDir = { 1.0f, -1.0f, 1.0f };
	ligDir.Normalize();
	g_sceneLight->SetDirectionLight(0, ligDir, { 5.0f, 5.0f, 5.0f});
	skyCube->SetLuminance(7.0f);
	skyCube->SetScale(20000.0f);
	
	//////////////////////////////////////
	// �������珉�������s���R�[�h���L�q����B
	//////////////////////////////////////

	//Game�N���X�̃I�u�W�F�N�g���쐬�B
	NewGO<Game>(0, "game");

	//////////////////////////////////////
	// ���������s���R�[�h�������̂͂����܂ŁI�I�I
	//////////////////////////////////////
	
	// ��������Q�[�����[�v�B
	while (DispatchWindowMessage())
	{
		Vector3 move;
		move += g_camera3D->GetRight() * g_pad[0]->GetLStickXF();
		move += g_camera3D->GetForward() * g_pad[0]->GetLStickYF();
		
		if (g_pad[0]->IsPress(enButtonUp)) {
			move.y = 1.0f;
		}
		if (g_pad[0]->IsPress(enButtonDown)) {
			move.y = -1.0f;
		}
		g_camera3D->Move(move * 10.0f);
		Quaternion qRot;
		qRot.SetRotationY(g_pad[0]->GetRStickXF() * 0.1f);
		g_camera3D->RotateOriginTarget(qRot);
		K2Engine::GetInstance()->Execute();

	}

	K2Engine::DeleteInstance();

#ifdef _DEBUG
	ReportLiveObjects();
#endif // _DEBUG
	return 0;
}

