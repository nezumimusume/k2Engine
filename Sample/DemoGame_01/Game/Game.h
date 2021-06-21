#pragma once

#include "level3DRender/LevelRender.h"


class Player;
class GameCamera;
class SkyCube;
class Background;
class Enemy;
class Lever;
class Door;
class SoundSource;

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
	void NotifyGameOver();
	//////////////////////////////////////
	// メンバ変数。
	//////////////////////////////////////
private:
	LevelRender				m_levelRender;
	Player*					m_player = nullptr;
	GameCamera*				m_gameCamera = nullptr;
	SkyCube*				m_skyCube = nullptr;
	Background*				m_background = nullptr;
	Enemy*					m_enemy = nullptr;
	std::vector<Lever*>		m_leverVector;
	std::vector<Door*>		m_doorVector;
	SoundSource*			m_bgm = nullptr;
};

