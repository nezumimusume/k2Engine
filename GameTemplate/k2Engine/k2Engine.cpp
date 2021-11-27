#include "k2EnginePreCompile.h"
#include "k2Engine.h"
#include "dbg/dbgVectorRenderer.h"

namespace nsK2Engine {
	K2Engine* K2Engine::m_instance = nullptr;
	SceneLight* g_sceneLight = nullptr;
	RenderingEngine* g_renderingEngine = nullptr;
	CollisionObjectManager* g_collisionObjectManager = nullptr;
	K2Engine* g_k2Engine = nullptr;
	void K2Engine::Init(const InitData& initData)
	{
		g_k2Engine = this;
		g_engine = &m_k2EngineLow;
		g_collisionObjectManager = &m_collisionObjectManager;
		g_renderingEngine = &m_renderingEngine;

		m_k2EngineLow.Init(initData.hwnd, initData.frameBufferWidth, initData.frameBufferHeight);
		m_renderingEngine.Init(initData.isSoftShadow);
		g_camera3D->SetPosition({ 0.0f, 100.0f, -200.0f });
		g_camera3D->SetTarget({ 0.0f, 50.0f, 0.0f });
#ifdef K2_DEBUG
		// �f�o�b�O�x�N�g�������_���[���쐬����B
		m_vectorRenderer = NewGO<nsDbg::VectorRenderer>(0);
#endif // #ifdef K2_DEBUG
	}
	K2Engine::~K2Engine()
	{
#ifdef K2_DEBUG
		DeleteGO(m_vectorRenderer);
#endif
		g_renderingEngine = nullptr;
		g_collisionObjectManager = nullptr;
		g_engine = nullptr;
	}

	void K2Engine::Execute()
	{
		auto& renderContext = g_graphicsEngine->GetRenderContext();

		g_engine->BeginFrame();

		g_engine->ExecuteUpdate();
		// �����_�����O�G���W���̍X�V�B
		m_renderingEngine.Update();
		
		g_engine->ExecuteRender();
		//�����_�����O�G���W�������s�B		
		m_renderingEngine.Execute(renderContext);
		
		//�����蔻��`��B
		g_engine->DebubDrawWorld();

		//////////////////////////////////////
		//�G��`���R�[�h�������̂͂����܂ŁI�I�I
		//////////////////////////////////////
		g_engine->EndFrame();
	}
#ifdef K2_DEBUG
	void K2Engine::DrawVector(const Vector3& vector, const Vector3& origin, const char* name)
	{
		nsDbg::VectorRenderer::SRenderData renderData;
		renderData.origin = origin;
		renderData.vector = vector;
		renderData.name = name;
		m_vectorRenderer->AddVector(renderData);
	}
	void K2Engine::SetDrawVectorEnable()
	{
		m_vectorRenderer->EnableRender();
	}
	void K2Engine::SetDrawVectorDisable()
	{
		m_vectorRenderer->DisableRender();
	}
#endif // #ifdef K2_DEBUG
}