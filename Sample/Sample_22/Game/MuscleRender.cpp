#include "stdafx.h"
#include "MuscleRender.h"
#include "Game.h"

bool MuscleRender::Start()
{
	m_animationClip.Load("Assets/animData/run.tka");
	m_animationClip.SetLoopFlag(true);

	m_modelRender.Init(
		"Assets/modelData/unityChan.tkm",
		&m_animationClip,
		1,
		enModelUpAxisY,
		true,
		m_maxMuscle
	);
	m_game = FindGO<Game>("game");
	return true;
}

void MuscleRender::Update()
{
	
}

void MuscleRender::Render(RenderContext& rc)
{
	if (m_game->m_isDrawInstancing == true)
	{
		m_modelRender.Draw(rc);
	}
}