#pragma once
#include "online/SyncOnlineTwoPlayerMatchEngine.h"

class Actor;
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
	/// <summary>
	/// キャラクターセレクトにもどる。
	/// </summary>
	void ReturnCharacterSelect();
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
	FontRender m_positionRender[2];
	int m_charaNo = -1;
	EnStep m_step = enStep_CharacterSelect;
	Actor* m_actor[2];
	SyncOnlineTwoPlayerMatchEngine* m_onlineTwoPlayerMatchEngine = nullptr; // 完全同期二人対戦用エンジン。
};

