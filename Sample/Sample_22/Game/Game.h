#pragma once

//レベルレンダーを使用するにはLevelRenderをインクルードする必要がある。
#include "level3DRender/LevelRender.h"

//クラス宣言。
class Player;
class GameCamera;
class Sphere;
class SphereRender;
class Muscle;
class MuscleRender;

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
	Player* m_player = nullptr;				//プレイヤー。
	GameCamera* m_gameCamera = nullptr;		//ゲームカメラ。
	LevelRender m_levelRender;				//レベルレンダー。
	std::vector<Sphere*> m_spheres;				//弾。
	SphereRender* m_sphereRender = nullptr;
	std::vector<Muscle*> m_muscles;				//あ。
	MuscleRender* m_muscleRender = nullptr;
	int m_maxSphere = 0;
	int m_maxMuscle = 0;
	bool m_isDrawInstancing = true;
};

