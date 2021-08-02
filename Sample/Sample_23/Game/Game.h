#pragma once

//レベルレンダーを使用するにはLevelRenderをインクルードする必要がある。
#include "level3DRender/LevelRender.h"

//クラス宣言。
class Player;
class GameCamera;
class Box;

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
	////////////////////////////////////
	// メンバ変数とか
	////////////////////////////////////
	Player*						m_player = nullptr;			//プレイヤー。
	GameCamera*					m_gameCamera = nullptr;		//ゲームカメラ。
	LevelRender					m_levelRender;				//レベルレンダー。
	std::vector<Box*>			m_boxs;						//ボックス。
	std::vector<PointLight*>	m_pointLightList;			//ポイントライト。
};

