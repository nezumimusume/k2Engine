#pragma once
#include "level3DRender/LevelRender.h"

//クラス宣言。
class Player;
class Background;
class GameCamera;
class Fade;
class Enemy;

class Game : public IGameObject
{
public:
	enum EnGameState
	{
		enGameState_DuringGamePlay,			//ゲームプレイ中。
		enGameState_GameOver,				//ゲームオーバー。
		enGameState_GameClear,				//ゲームクリア。
		enGameState_GameClear_Idle			//ゲームクリア(待機中)。
	};
public:
	//////////////////////////////////////
	// メンバ関数。
	//////////////////////////////////////
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	/// <summary>
	/// 獲得した☆の数を+1する。
	/// </summary>
	void AddGetStarCount()
	{
		m_starCount++;
	}
	/// <summary>
	/// ゲームオーバーを通知する。
	/// </summary>
	void NotifyGameOver();
	/// <summary>
	/// ゲームクリアを通知する。
	/// </summary>
	void NotifyGameClear();
	void NotifyGameClearIdle();
	/// <summary>
	/// リスタートを通知する。
	/// </summary>
	void NotifyReStart();
private:
	/// <summary>
	/// フォントの更新。
	/// </summary>
	void UpdateFont();
	/// <summary>
	/// タイマーを加算する。
	/// </summary>
	void CountTimer();
	/// <summary>
	/// 空を初期化。
	/// </summary>
	void InitSky();
private:
	Player*					m_player = nullptr;			//プレイヤー。
	Background*				m_background = nullptr;		//背景。
	GameCamera*				m_gameCamera = nullptr;		//ゲームカメラ。
	std::vector<Enemy*>		m_enemys;			//エネミーのリスト。
	LevelRender				m_levelRender;				//レベル。
	SkyCube*				m_skyCube = nullptr;		//スカイキューブ。
	bool					m_isWaitFadeout = false;
	Fade*					m_fade = nullptr;			//フェード。
	int						m_starCount = 0;			//獲得した☆の数。
	FontRender				m_starCountFont;			//獲得した☆の数を表示するフォント。
	Vector3					m_starCountFontPosition;
	float					m_timer = 0.0f;				//タイマー。
	FontRender				m_timerFont;				//タイマーを表示するフォント。
	EnGameState				m_gameState = enGameState_DuringGamePlay;		//ゲームステート。
	SpriteRender			m_pressA;
	std::vector<PointLight*> m_pointLightList;
	std::vector<SpotLight*> m_spotLightList;
	std::vector<VolumeSpotLight*> m_volumeSpotLightList;
	std::vector< VolumePointLight*> m_volumePointLightList;
	int m_skyCubeType = enSkyCubeType_Day;
	SoundSource* m_bgm = nullptr;
};


