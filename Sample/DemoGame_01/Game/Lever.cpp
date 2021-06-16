#include "stdafx.h"
#include "Lever.h"

#include "Door.h"

#include "collision/CollisionObject.h"

Lever::Lever()
{

}

Lever::~Lever()
{
	DeleteGO(m_collisionObject);
}

bool Lever::Start()
{
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/lever/idle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(false);
	m_animationClips[enAnimationClip_On].Load("Assets/animData/lever/on.tka");
	m_animationClips[enAnimationClip_On].SetLoopFlag(false);
	m_modelRender.Init("Assets/modelData/lever/lever.tkm", m_animationClips, enAnimationClip_Num);
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetScale(m_scale);
	m_modelRender.SetRotation(m_rotation);

	//当たり判定用のコリジョンを作成。
	m_collisionObject = NewGO<CollisionObject>(0);
	m_collisionObject->CreateBox(m_position, m_rotation, Vector3::One * 70.0f);
	m_collisionObject->SetName("lever");
	m_collisionObject->SetIsEnableAutoDelete(false);
	return true;
}

void Lever::Update()
{
	PushLever();
	PlayAnimation();
	OpenDoor();

	m_modelRender.Update();
}

void Lever::PushLever()
{
	auto collisions = g_collisionObjectManager->FindCollisionObjects("push_lever");

	for (auto collision : collisions)
	{
		if (collision->IsHit(*m_collisionObject) == true)
		{
			m_leverState = 1;
		}
	}
}

void Lever::PlayAnimation()
{
	switch (m_leverState)
	{
	case 0:
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;
	case 1:
		m_modelRender.PlayAnimation(enAnimationClip_On);
		break;
	default:
		break;
	}
}

void Lever::OpenDoor()
{
	if (m_leverState == 1)
	{
		if (m_modelRender.IsPlayingAnimation() == false)
		{
			auto door = FindGO<Door>("door");
			door->Open();
		}
	}
}

void Lever::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}