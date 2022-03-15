#include "stdafx.h"
#include "Muscle.h"
#include "MuscleRender.h"
#include "Game.h"

Muscle::Muscle()
{

}

Muscle::~Muscle()
{
	m_muscleRender->RemoveInstance(m_instanceNo);
}

bool Muscle::Start()
{
	m_muscleRender = FindGO<MuscleRender>("musclerender");

	m_game = FindGO<Game>("game");

	m_animationClip.Load("Assets/animData/sample2/muscle.tka");
	m_animationClip.SetLoopFlag(true);

	m_modelRender.Init(
		"Assets/modelData/sample2.tkm",
		&m_animationClip,
		1,
		enModelUpAxisZ);
	m_modelRender.SetTRS(m_position, m_rotation, m_scale);
	m_modelRender.Update();
	return true;
}

void Muscle::Update()
{
	if (m_game->m_isDrawInstancing == true)
	{
		m_muscleRender->UpdateInstancingData(
			m_instanceNo,
			m_position,
			m_rotation,
			m_scale
		);
	}
	else
	{
		m_modelRender.Update();
	}
	
}

void Muscle::Render(RenderContext& rc)
{
	if (m_game->m_isDrawInstancing == false)
	{
		m_modelRender.Draw(rc);
	}
}