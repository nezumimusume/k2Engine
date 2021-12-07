#include "k2EngineLowPreCompile.h"
#include "k2EngineLow.h"
#include "graphics/Texture.h"

namespace nsK2EngineLow {
	K2EngineLow* g_engine = nullptr;
	GameTime* g_gameTime = nullptr;

	K2EngineLow::~K2EngineLow()
	{
		// �O���[�o���ȃA�N�Z�X�|�C���g��nullptr�����B
		g_graphicsEngine = nullptr;
		g_gameTime = nullptr;
		
		delete m_graphicsEngine;
		
		//�Q�[���I�u�W�F�N�g�}�l�[�W���[���폜�B
		GameObjectManager::DeleteInstance();
		PhysicsWorld::DeleteInstance();
		EffectEngine::DeleteInstance();

		delete g_soundEngine;
	}
	void K2EngineLow::Init(HWND hwnd, UINT frameBufferWidth, UINT frameBufferHeight)
	{
		//�O���t�B�b�N�G���W���̏������B
		m_graphicsEngine = new GraphicsEngine();
		m_graphicsEngine->Init(hwnd, frameBufferWidth, frameBufferHeight);
		g_gameTime = &m_gameTime;
		//�Q�[���p�b�h�̏������B
		for (int i = 0; i < GamePad::CONNECT_PAD_MAX; i++) {
			g_pad[i] = &m_pad[i];
		}

		GameObjectManager::CreateInstance();
		PhysicsWorld::CreateInstance();
		g_soundEngine = new SoundEngine();
		//�G�t�F�N�g�G���W���̏������B
		EffectEngine::CreateInstance();
	}
	void K2EngineLow::BeginFrame()
	{
		m_fpsLimitter.BeginFrame();
		m_gameTime.BeginMeasurement();
		m_graphicsEngine->BeginRender();
		EffectEngine::GetInstance()->BeginFrame();
		for (auto& pad : m_pad) {
			pad.BeginFrame();
		}

	}
	void K2EngineLow::EndFrame()
	{
		m_graphicsEngine->EndRender();
		m_fpsLimitter.Wait();
		m_gameTime.EndMeasurement();

	}

	void K2EngineLow::ExecuteUpdate()
	{
		for (auto& pad : m_pad) {
			pad.Update();
		}
		g_soundEngine->Update();
		GameObjectManager::GetInstance()->ExecuteUpdate();
		// �G�t�F�N�g�G���W���̍X�V�B
		EffectEngine::GetInstance()->Update(g_gameTime->GetFrameDeltaTime());
	}
	/// <summary>
	/// �`�揈�������s�B
	/// </summary>
	void K2EngineLow::ExecuteRender()
	{
		auto& renderContext = g_graphicsEngine->GetRenderContext();
		// �Q�[���I�u�W�F�N�g�}�l�[�W���[�̕`�揈�������s�B
		GameObjectManager::GetInstance()->ExecuteRender(renderContext);
		
	}

	/// <summary>
	/// �����蔻��`�揈�������s�B
	/// </summary>
	void K2EngineLow::DebubDrawWorld()
	{
		auto& renderContext = g_graphicsEngine->GetRenderContext();
		//�����蔻��`�揈�������s�B
		PhysicsWorld::GetInstance()->DebubDrawWorld(renderContext);
	}
}