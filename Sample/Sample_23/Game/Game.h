#pragma once
#include "level3DRender/LevelRender.h"

//クラス宣言。
class Player;
class GameCamera;
class Sphere;

class Game : public IGameObject
{
public:
	
public:
	//////////////////////////////////////
	// メンバ関数。
	//////////////////////////////////////
	Game();
	~Game();
	bool Start();
	void Update();
private:
	/// <summary>
	/// 空を初期化。
	/// </summary>
	void InitSky();
private:
	Player*					m_player = nullptr;			//プレイヤー。
	GameCamera*				m_gameCamera = nullptr;		//ゲームカメラ。
	SkyCube*				m_skyCube = nullptr;		//スカイキューブ。
	int						m_skyCubeType = enSkyCubeType_Day;
	Sphere*					m_sphere = nullptr;
};

