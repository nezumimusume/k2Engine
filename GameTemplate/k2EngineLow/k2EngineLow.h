#pragma once

#include "HID/GamePad.h"
#include "util/TResourceBank.h"
#include "tkFile/TkmFile.h"
#include "graphics/Shader.h"
#include "time/FPSLimitter.h"

namespace nsK2EngineLow {
	class GraphicsEngine;
	class GameTime;
	class Texture;

	class K2EngineLow {
	public:
		/// <summary>
		/// デストラクタ。
		/// </summary>
		~K2EngineLow();
		/// <summary>
		/// 更新処理を実行。
		/// </summary>
		void ExecuteUpdate();
		/// <summary>
		/// 描画処理を実行。
		/// </summary>
		void ExecuteRender();
		
		/// <summary>
		/// フレームの開始時に呼ばれる処理。
		/// </summary>
		void BeginFrame();
		/// <summary>
		/// フレームの終了時に呼ばれる処理。
		/// </summary>
		void EndFrame();

		/// <summary>
		/// ゲームエンジンの初期化。
		/// </summary>
		void Init(HWND hwnd, UINT frameBufferWidth, UINT frameBufferHeight);
		/// <summary>
		/// tkmファイルをバンクから取得。
		/// </summary>
		/// <param name="filePath">ファイルパス</param>
		/// <returns>tkmファイル。nullptrが返ってきたらバンクに登録されていない。</returns>
		TkmFile* GetTkmFileFromBank(const char* filePath)
		{
			return m_tkmFileBank.Get(filePath);
		}
		/// <summary>
		/// tkmファイルをバンクに登録。
		/// </summary>
		/// <param name="filePath">tkmファイルパス</param>
		/// <param name="tkmFile"></param>
		void RegistTkmFileToBank(const char* filePath, TkmFile* tkmFile)
		{
			m_tkmFileBank.Regist(filePath, tkmFile);
		}
		/// <summary>
		/// シェーダーファイルバンクからシェーダーを取得。
		/// </summary>
		/// <param name="filePath"></param>
		/// <returns></returns>
		Shader* GetShaderFromBank(const char* filePath, const char* entryPointFuncName)
		{
			std::string programName = filePath;
			programName += entryPointFuncName;
			return m_shaderBank.Get(programName.c_str());
		}
		/// <summary>
		/// テクスチャをバンクに登録。
		/// </summary>
		/// <param name="filePath">ファイルパス。</param>
		/// <param name="texture">テクスチャ。</param>
		void RegistTextureToBank(const char* filePath, Texture* texture)
		{
			m_textureBank.Regist(filePath, texture);
		}
		/// <summary>
		/// バンクからテクスチャを取得。
		/// </summary>
		/// <param name="filePath">ファイルパス。</param>
		/// <returns>テクスチャ。</returns>
		Texture* GetTextureFromBank(const char* filePath)
		{
			return m_textureBank.Get(filePath);
		}
		/// <summary>
		/// 
		/// </summary>
		/// <param name="filePath"></param>
		/// <param name="shader"></param>
		void RegistShaderToBank(const char* filePath, const char* entryPointFuncName, Shader* shader)
		{
			std::string programName = filePath;
			programName += entryPointFuncName;
			m_shaderBank.Regist(programName.c_str(), shader);
		}
		/// <summary>
		/// 現在設定されている最大FPSを取得。
		/// </summary>
		/// <returns></returns>
		int GetMaxFPS() const
		{
			return m_fpsLimitter.GetMaxFPS();
		}
		/// <summary>
		/// 最大FPSを設定。
		/// </summary>
		/// <param name="maxFps"></param>
		void SetMaxFPS(int maxFPS)
		{
			m_fpsLimitter.SetMaxFPS(maxFPS);
		}
	private:
		GraphicsEngine* m_graphicsEngine = nullptr;		// グラフィックエンジン。
		TResourceBank<TkmFile> m_tkmFileBank;			// tkmファイルバンク。
		TResourceBank<Shader> m_shaderBank;				// シェーダーバンク
		TResourceBank<Texture>	m_textureBank;			// テクスチャバンク。
		GamePad m_pad[GamePad::CONNECT_PAD_MAX];		// ゲームパッド。
		GameTime m_gameTime;
		FPSLimitter m_fpsLimitter;						// FPSに制限をかける処理。
	};

	extern K2EngineLow* g_engine;	// 低レベルK2エンジン。
	extern GameTime* g_gameTime;
}

