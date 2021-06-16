#include "stdafx.h"
#include "Door.h"

Door::Door()
{

}

Door::~Door()
{

}

bool Door::Start()
{

	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/door/idle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Open].Load("Assets/animData/door/open.tka");
	m_animationClips[enAnimationClip_Open].SetLoopFlag(false);
	m_modelRender.Init("Assets/modelData/door/door.tkm", m_animationClips, enAnimationClip_Num);
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetScale(m_scale);
	m_modelRender.SetRotation(m_rotation);

	m_modelRender.Update();
	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());
	return true;
}

void Door::Update()
{
	PlayAnimation();
	ReleasePhysicsObject();

	m_modelRender.Update();
	
}

void Door::Open()
{
	m_doorState = 1;
}

void Door::PlayAnimation()
{
	switch (m_doorState)
	{
	case 0:
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;
	case 1:
		m_modelRender.PlayAnimation(enAnimationClip_Open);
		break;
	default:
		break;
	}
	//m_modelRender.PlayAnimation(enAnimationClip_Open);
}

void Door::ReleasePhysicsObject()
{
	if (m_doorState == 1)
	{
		if (m_modelRender.IsPlayingAnimation() == false)
		{
			m_physicsStaticObject.Release();
		}
	}
}

void Door::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}

