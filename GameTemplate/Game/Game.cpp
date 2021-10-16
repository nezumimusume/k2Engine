#include "stdafx.h"
#include "Game.h"
#include "geometry/BSP.h"


bool Game::Start()
{
	BSP bsp;
	AABB aabb[5];
	
	// ìKìñÇ…AABBÇç\ízÇ∑ÇÈÅB	
	
	
	bsp.AddLeaf({ 100, 100, 100 });
	bsp.AddLeaf({ 200, 110, 300 });
	bsp.AddLeaf({ 130, 30,  300 });
	bsp.AddLeaf({ 200, 200, 200 });
	bsp.AddLeaf({ 100, 100, 100 });

/*	bvh.AddLeaf({-50, -40, -40});
	bvh.AddLeaf({ -100,-20, -100 });
	bvh.AddLeaf({ -50, -40, -400 });
	bvh.AddLeaf({ -5, -100, -40 });
	bvh.AddLeaf({ -200, -200, -300 });
	*/
	bsp.Build();

	m_modelRender.Init("Assets/modelData/test.tkm");
	return true;
}

void Game::Update()
{
	m_modelRender.Update();
}

void Game::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}