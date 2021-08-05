#include "stdafx.h"
#include "MuscleRender.h"
#include "Game.h"

bool MuscleRender::Start()
{
	m_animationClip.Load("Assets/animData/sample2/muscle.tka");
	m_animationClip.SetLoopFlag(true);

	m_modelRender.Init(
		"Assets/modelData/sample2.tkm",
		&m_animationClip,
		1,
		enModelUpAxisZ,
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