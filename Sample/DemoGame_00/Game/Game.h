#pragma once
#include "level3DRender/LevelRender.h"

//クラス宣言。
class Player;
class Background;
class GameCamera;
class SkyCube;

class Game : public IGameObject
{
public:
	//////////////////////////////////////
	// メンバ関数。
	//////////////////////////////////////
	Game();
	~Game();
	bool Start();

	////////////////////////////////////
	// メンバ変数とか
	////////////////////////////////////
	Player*		m_player = nullptr;			//プレイヤー。
	Background* m_background = nullptr;		//背景。
	GameCamera* m_gameCamera = nullptr;		//ゲームカメラ。
	LevelRender	m_levelRender;				//レベル。
	SkyCube*	m_skyCube = nullptr;		//スカイキューブ。
};

