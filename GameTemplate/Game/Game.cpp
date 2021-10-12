#include "stdafx.h"
#include "Game.h"
#include "geometry/BVH.h"

bool Game::Start()
{
	BVH bvh;
	AABB aabb[5];
	
	// ìKìñÇ…AABBÇç\ízÇ∑ÇÈÅB	
	aabb[0].Init({ 100, 100, 100 }, { -50, -40, -40 });
	aabb[1].Init({ 200, 110, 300 }, { -100,-20, -100 });
	aabb[2].Init({ 130, 30,  300 }, { -50, -40, -400 });
	aabb[3].Init({ 200, 200, 200 }, { -5, -100, -40 });
	aabb[4].Init({ 100, 100, 100 }, { -200, -200, -300 });
	
	bvh.AddLeaf(aabb[0]);
	bvh.AddLeaf(aabb[1]);
	bvh.AddLeaf(aabb[2]);
	bvh.AddLeaf(aabb[3]);
	bvh.AddLeaf(aabb[4]);

	bvh.Build();

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