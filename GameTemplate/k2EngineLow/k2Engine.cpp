#include "k2EngineLowPreCompile.h"
#include "k2Engine.h"
#include "graphics/GraphicsEngine.h"

K2Engine* g_engine = nullptr;
GameTime* g_gameTime = nullptr;

K2Engine::~K2Engine()
{
	if (m_graphicsEngine) {
		delete m_graphicsEngine;
	}
}
void K2Engine::Init(HWND hwnd, UINT frameBufferWidth, UINT frameBufferHeight)
{
	//グラフィックエンジンの初期化。
	m_graphicsEngine = new GraphicsEngine();
	m_graphicsEngine->Init(hwnd, frameBufferWidth, frameBufferHeight);
	g_gameTime = &m_gameTime;
	//ゲームパッドの初期化。
	for (int i = 0; i < GamePad::CONNECT_PAD_MAX; i++) {
		g_pad[i] = &m_pad[i];
	}
}
void K2Engine::BeginFrame()
{
	m_gameTime.BeginMeasurement();
	m_graphicsEngine->BeginRender();
	for (auto& pad : m_pad) {
		pad.BeginFrame();
		pad.Update();
	}
	
}
void K2Engine::EndFrame()
{
	m_graphicsEngine->EndRender();
	m_gameTime.EndMeasurement();
}