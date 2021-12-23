#include "stdafx.h"
#include "Dancer.h"

Dancer::Dancer()
{

}

Dancer::~Dancer()
{

}

bool Dancer::Start()
{
	m_animationClip.Load("Assets/animData/maria/dance.tka");
	m_animationClip.SetLoopFlag(true);

	m_modelRender.Init("Assets/modelData/maria.tkm", &m_animationClip, 1);

	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);
	m_modelRender.SetScale(m_scale);
	return true;
}

void Dancer::Update() 
{
	m_modelRender.Update();
}

void Dancer::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}
