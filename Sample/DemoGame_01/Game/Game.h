#pragma once

#include "level3DRender/LevelRender.h"


class Player;
class GameCamera;
class SkyCube;
class Background;
class Lever;
class Door;
class SoundSource;
class Fade;

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
	void NotifyGameClear();
	void NotifyGameOver();
	const bool IsWannihilationEnemy() const
	{
		return m_numEnemy == m_numDefeatedEnemy;
	}
	void AddDefeatedEnemyNumber()
	{
		m_numDefeatedEnemy++;
	}
	//////////////////////////////////////
	// メンバ変数。
	//////////////////////////////////////
private:
	LevelRender				m_levelRender;
	Player*					m_player = nullptr;
	GameCamera*				m_gameCamera = nullptr;
	SkyCube*				m_skyCube = nullptr;
	Background*				m_background = nullptr;
	std::vector<Lever*>		m_leverVector;
	std::vector<Door*>		m_doorVector;
	SoundSource*			m_bgm = nullptr;
	Fade*					m_fade = nullptr;
	bool					m_isWaitFadeout = false;
	int						m_numEnemy = 0;
	int						m_numDefeatedEnemy = 0;
};

