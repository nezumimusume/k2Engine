#pragma once

#include "Level3DRender/LevelRender.h"

class Player;

class Game : public IGameObject
{
public:
	Game() {}
	~Game() {}
	bool Start();
	void Render(RenderContext& rc);

private:
	Player* m_player;
	LevelRender m_levelRender;
};

