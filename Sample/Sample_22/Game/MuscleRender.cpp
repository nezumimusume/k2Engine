#include "stdafx.h"
#include "MuscleRender.h"

bool MuscleRender::Start()
{
	m_animationClip.Load("Assets/animData/sample2/muscle.tka");
	m_animationClip.SetLoopFlag(true);

	m_modelRender.Init(
		"Assets/modelData/sample2.tkm",
		&m_animationClip,
		1,
		enModelUpAxisZ,
		false,
		m_maxMuscle
	);
	return true;
}
void MuscleRender::Update()
{
}

void MuscleRender::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}