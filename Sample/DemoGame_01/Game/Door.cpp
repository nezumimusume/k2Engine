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
	m_animationClips[enAnimationClip_Close].Load("Assets/animData/door/close.tka");
	m_animationClips[enAnimationClip_Close].SetLoopFlag(false);
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
	ManageState();

	m_modelRender.Update();
}

void Door::NotifyOpen()
{
	m_doorState = enDoorState_Open;
}

void Door::NotifyClose()
{
	m_doorState = enDoorState_Close;
}

void Door::PlayAnimation()
{
	switch (m_doorState)
	{
	case enDoorState_Idle:
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;
	case enDoorState_Open:
		m_modelRender.PlayAnimation(enAnimationClip_Open);
		break;
	case enDoorState_Close:
		m_modelRender.PlayAnimation(enAnimationClip_Close);
		break;
	default:
		break;
	}
}

void Door::ReleasePhysicsObject()
{
	m_physicsStaticObject.Release();
}

void Door::CreatePhysicsObject()
{
	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());
}

void Door::ProcessOpenStateTransition()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		ReleasePhysicsObject();
		m_doorState = enDoorState_Open_Idle;
	}
}

void Door::ProcessCloseStateTransition()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		CreatePhysicsObject();
		m_doorState = enDoorState_Close_Idle;
	}
}

void Door::ManageState()
{
	switch (m_doorState)
	{
	case Door::enDoorState_Idle:
		
		break;
	case Door::enDoorState_Open:
		ProcessOpenStateTransition();
		break;
	case Door::enDoorState_Open_Idle:
		
		break;
	case Door::enDoorState_Close:
		ProcessCloseStateTransition();
		break;
	case Door::enDoorState_Close_Idle:
		
		break;
	default:
		break;
	}
}

void Door::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}

