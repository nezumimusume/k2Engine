#pragma once

#include "Level3DRender/LevelRender.h"

class Player;

class Game : public IGameObject
{
public:
	Game() {}
	~Game() {}
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	Vector3 pos;
	ModelRender m_modelRender;
	ModelRender m_bgRender;
	ModelRender m_groundRender;
};

