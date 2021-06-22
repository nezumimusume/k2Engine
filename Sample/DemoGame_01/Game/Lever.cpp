#include "stdafx.h"
#include "Lever.h"

#include "Door.h"

#include "collision/CollisionObject.h"
#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"

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
	m_animationClips[enAnimationClip_Off].Load("Assets/animData/lever/off.tka");
	m_animationClips[enAnimationClip_Off].SetLoopFlag(false);
	m_modelRender.Init("Assets/modelData/lever/lever.tkm", m_animationClips, enAnimationClip_Num);
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetScale(m_scale);
	m_modelRender.SetRotation(m_rotation);

	//当たり判定用のコリジョンを作成。
	m_collisionObject = NewGO<CollisionObject>(0);
	m_collisionObject->CreateBox(m_position, m_rotation, Vector3::One * 70.0f);
	m_collisionObject->SetName("lever");
	m_collisionObject->SetIsEnableAutoDelete(false);

	g_soundEngine->ResistWaveFileBank(6, "Assets/sound/lever.wav");
	return true;
}

void Lever::Update()
{
	PlayAnimation();
	ManageState();
	m_modelRender.Update();
}

void Lever::ProcessTransitionPushState()
{
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("player_lever");

	for (auto collision : collisions)
	{
		if (collision->IsHit(m_collisionObject) == true)
		{
			m_leverState = enLeverState_Push;
			SoundSource* se = NewGO<SoundSource>(0);
			se->Init(6);
			se->Play(false);
			se->SetVolume(0.7f);
		}
	}
}

void Lever::ProcessTransitionPullState()
{
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("player_lever");

	for (auto collision : collisions)
	{
		if (collision->IsHit(m_collisionObject) == true)
		{
			m_leverState = enLeverState_Pull;
			SoundSource* se = NewGO<SoundSource>(0);
			se->Init(6);
			se->Play(false);
			se->SetVolume(0.7f);
			break;
		}
	}
}

void Lever::PlayAnimation()
{
	switch (m_leverState)
	{
	case enLeverState_Idle:
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;
	case enLeverState_Push:
		m_modelRender.PlayAnimation(enAnimationClip_On);
		break;
	case enLeverState_Pull:
		m_modelRender.PlayAnimation(enAnimationClip_Off);
		break;
	default:
		break;
	}
}

void Lever::ProcessTransitionPushIdleState()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		auto doors = FindGOs<Door>("door");
		for (auto door : doors)
		{
			//番号が同じなら、ドアをオープン。
			if (m_leverNumber == door->GetDoorNumber())
			{
				door->NotifyOpen();
				m_leverState = enLeverState_Push_Idle;
				break;
			}
		}
	}
}

void Lever::ProcessTransitionPullIdleState()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		auto doors = FindGOs<Door>("door");
		for (auto door : doors)
		{
			//番号が同じなら、ドアをクローズ。
			if (m_leverNumber == door->GetDoorNumber())
			{
				door->NotifyClose();
				m_leverState = enLeverState_Pull_Idle;
				break;
			}
		}
	}
}

void Lever::ProcessIdleStateTransition()
{
	//待機状態から押す状態への遷移。
	ProcessTransitionPushState();
}

void Lever::ProcessPushStateTransition()
{
	//押す状態から待機状態への遷移。
	ProcessTransitionPushIdleState();
}

void Lever::ProcessPushIdleStateTransition()
{
	//待機状態から引く状態への遷移。
	ProcessTransitionPullState();
}

void Lever::ProcessPullStateTransition()
{
	//引く状態から待機状態への遷移。
	ProcessTransitionPullIdleState();
}

void Lever::ProcessPullIdleStateTransition()
{
	//待機状態から押す状態への遷移。
	ProcessTransitionPushState();
}

void Lever::ManageState()
{
	switch (m_leverState)
	{
	case enLeverState_Idle:
		ProcessIdleStateTransition();
		break;
	case enLeverState_Push:
		ProcessPushStateTransition();
		break;
	case enLeverState_Push_Idle:
		ProcessPushIdleStateTransition();
		break;
	case enLeverState_Pull:
		ProcessPullStateTransition();
		break;
	case enLeverState_Pull_Idle:
		ProcessPullIdleStateTransition();
		break;
	default:
		break;
	}
}

void Lever::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}