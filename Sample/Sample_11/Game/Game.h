#pragma once

//クラス宣言。
class Player;
class Background;
class GameCamera;

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
	Player* m_player = nullptr;				//プレイヤー。
	Background* m_background = nullptr;		//背景。
	GameCamera* m_gameCamera = nullptr;		//ゲームカメラ。
};

