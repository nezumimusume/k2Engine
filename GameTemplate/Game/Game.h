#pragma once

#include "Level3D/Level.h"

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
	Level m_level;
};

