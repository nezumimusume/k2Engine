#include "k2EngineLowPreCompile.h"
#include "k2EngineLow.h"
#include "graphics/Texture.h"

namespace nsK2EngineLow {
	K2EngineLow* g_engine = nullptr;
	GameTime* g_gameTime = nullptr;

	K2EngineLow::~K2EngineLow()
	{
		// グローバルなアクセスポイントにnullptrを代入。
		g_graphicsEngine = nullptr;
		g_gameTime = nullptr;
		
		delete m_graphicsEngine;
		
		//ゲームオブジェクトマネージャーを削除。
		GameObjectManager::DeleteInstance();
		PhysicsWorld::DeleteInstance();
		EffectEngine::DeleteInstance();

		delete g_soundEngine;
	}
	void K2EngineLow::Init(HWND hwnd, UINT frameBufferWidth, UINT frameBufferHeight)
	{
		//グラフィックエンジンの初期化。
		m_graphicsEngine = new GraphicsEngine();
		m_graphicsEngine->Init(hwnd, frameBufferWidth, frameBufferHeight);
		g_gameTime = &m_gameTime;
		//ゲームパッドの初期化。
		for (int i = 0; i < GamePad::CONNECT_PAD_MAX; i++) {
			g_pad[i] = &m_pad[i];
		}

		GameObjectManager::CreateInstance();
		PhysicsWorld::CreateInstance();
		g_soundEngine = new SoundEngine();
		//エフェクトエンジンの初期化。
		EffectEngine::CreateInstance();
	}
	void K2EngineLow::BeginFrame()
	{
		m_fpsLimitter.BeginFrame();
		m_gameTime.BeginMeasurement();
		m_graphicsEngine->BeginRender();
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
		// エフェクトエンジンの更新。
		EffectEngine::GetInstance()->Update(g_gameTime->GetFrameDeltaTime());
	}
	/// <summary>
	/// 描画処理を実行。
	/// </summary>
	void K2EngineLow::ExecuteRender()
	{
		auto& renderContext = g_graphicsEngine->GetRenderContext();
		// ゲームオブジェクトマネージャーの描画処理を実行。
		GameObjectManager::GetInstance()->ExecuteRender(renderContext);
		
	}

	/// <summary>
	/// 当たり判定描画処理を実行。
	/// </summary>
	void K2EngineLow::DebubDrawWorld()
	{
		auto& renderContext = g_graphicsEngine->GetRenderContext();
		//当たり判定描画処理を実行。
		PhysicsWorld::GetInstance()->DebubDrawWorld(renderContext);
	}
}