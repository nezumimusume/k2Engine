#pragma once

//Level2Dを使用するにはファイルをインクルードする必要がある。
#include "level2DRender/Level2DRender.h"

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
	//////////////////////////////////////
	// メンバ変数。
	//////////////////////////////////////
	ModelRender		m_modelRender;
	Level2DRender	m_level2DRender;
	SpriteRender	m_spriteRender;
};

