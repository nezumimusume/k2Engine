#pragma once
#include "online/SyncOnlineTwoPlayerMatchEngine.h"
class Game : public IGameObject
{
public:
	//////////////////////////////////////
	// メンバ関数。
	//////////////////////////////////////
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
private:
	/// <summary>
	/// すべてのプレイヤーがルームにジョインしたときに呼ばれる処理。
	/// </summary>
	/// <param name="pData"></param>
	/// <param name="size"></param>
	void OnAllPlayerJoined(void* pData, int size);
	/// <summary>
	/// すべてのプレイヤーがゲームスタートした時に呼ばれる処理。
	/// </summary>
	void OnAllPlayerStartGame();
	/// <summary>
	/// 通信エラーが起きた。
	/// </summary>
	void OnError();
private:
	//////////////////////////////////////
	// メンバ変数。
	//////////////////////////////////////
	enum EnStep {
		enStep_CharacterSelect,			// キャラクターセレクト。
		enStep_WaitAllPlayerJoined,		// すべてのプレイヤーが揃うまで待つ。
		enStep_WaitAllPlayerStartGame,	// すべてのプレイヤーがゲーム開始するのを待つ。
		enStep_InGame,					// インゲーム。
		enStep_Error,					// エラー。
	};
	FontRender m_fontRender;
	EnStep m_step = enStep_CharacterSelect;
	ModelRender m_modelRender[2];
	Vector3 pos[2];
	SyncOnlineTwoPlayerMatchEngine* m_onlineTwoPlayerMatchEngine = nullptr; // 完全同期二人対戦用エンジン。
};

