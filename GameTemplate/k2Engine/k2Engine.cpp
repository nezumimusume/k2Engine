#include "k2EnginePreCompile.h"
#include "k2Engine.h"

namespace nsK2Engine {
	K2Engine* K2Engine::m_instance = nullptr;
	void K2Engine::Init(const InitData& initData)
	{
		g_engine = new K2EngineLow;
		g_engine->Init(initData.hwnd, initData.frameBufferWidth, initData.frameBufferHeight);
		g_camera3D->SetPosition({ 0.0f, 100.0f, -200.0f });
		g_camera3D->SetTarget({ 0.0f, 50.0f, 0.0f });

		g_collisionObjectManager = new CollisionObjectManager();
		// �����_�����O�G���W����������
		RenderingEngine::CreateInstance(initData.isSoftShadow);
	}
	K2Engine::~K2Engine()
	{
		// �����_�����O�G���W���̍폜
		RenderingEngine::DeleteInstance();
		delete g_collisionObjectManager;
		g_collisionObjectManager = nullptr;
		// k2Engine���폜
		delete g_engine;
	}
	
	void K2Engine::Execute()
	{
		auto& renderContext = g_graphicsEngine->GetRenderContext();

		g_engine->BeginFrame();

		g_engine->ExecuteUpdate();
		// �����_�����O�G���W���̍X�V�B
		RenderingEngine::GetInstance()->Update();


		g_engine->ExecuteRender();
		//�����_�����O�G���W�������s�B		
		RenderingEngine::GetInstance()->Execute(renderContext);

		//////////////////////////////////////
		//�G��`���R�[�h�������̂͂����܂ŁI�I�I
		//////////////////////////////////////
		g_engine->EndFrame();
	}
}