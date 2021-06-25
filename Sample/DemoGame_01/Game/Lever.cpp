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
	//コリジョンオブジェクトを削除する。
	DeleteGO(m_collisionObject);
}

bool Lever::Start()
{
	//アニメーションを読み込む。
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/lever/idle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Push].Load("Assets/animData/lever/push.tka");
	m_animationClips[enAnimationClip_Push].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Pull].Load("Assets/animData/lever/pull.tka");
	m_animationClips[enAnimationClip_Pull].SetLoopFlag(false);
	//モデルを読み込む。
	m_modelRender.Init("Assets/modelData/lever/lever.tkm", m_animationClips, enAnimationClip_Num);
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetScale(m_scale);
	m_modelRender.SetRotation(m_rotation);

	//コリジョンオブジェクトを作成。
	m_collisionObject = NewGO<CollisionObject>(0);
	//ボックス状のコリジョンを作成。
	m_collisionObject->CreateBox(m_position, m_rotation, Vector3::One * 70.0f);
	m_collisionObject->SetName("lever");
	m_collisionObject->SetIsEnableAutoDelete(false);

	//音を読み込む。
	g_soundEngine->ResistWaveFileBank(6, "Assets/sound/lever.wav");
	return true;
}

void Lever::Update()
{
	//アニメーションを再生する。
	PlayAnimation();
	//ステート管理。
	ManageState();

	//モデルの更新。
	m_modelRender.Update();
}

void Lever::ProcessTransitionPushState()
{
	//プレイヤーが作成した、レバー用のコリジョンの配列を取得。
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("player_lever");

	//for文で配列を回す。
	for (auto collision : collisions)
	{
		//レバー自身のコリジョンとプレイヤーの作成したコリジョンが衝突したら。
		if (collision->IsHit(m_collisionObject) == true)
		{
			//押すステートに遷移させる。
			m_leverState = enLeverState_Push;
			//効果音を流す。
			SoundSource* se = NewGO<SoundSource>(0);
			se->Init(6);
			se->Play(false);
			se->SetVolume(0.6f);
		}
	}
}

void Lever::ProcessTransitionPullState()
{
	//プレイヤーが作成した、レバー用のコリジョンの配列を取得。
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("player_lever");

	//for文で配列を回す。
	for (auto collision : collisions)
	{
		//レバー自身のコリジョンとプレイヤーの作成したコリジョンが衝突したら。
		if (collision->IsHit(m_collisionObject) == true)
		{
			//引くステートに遷移する。
			m_leverState = enLeverState_Pull;
			//効果音を流す。
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
	//待機ステートなら。
	case enLeverState_Idle:
		//待機アニメーションを再生する。
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;
	//押すステートの時。
	case enLeverState_Push:
		//押すアニメーションを再生する。
		m_modelRender.PlayAnimation(enAnimationClip_Push);
		break;
	//引くステートのとき。
	case enLeverState_Pull:
		//引くアニメーションを再生する。
		m_modelRender.PlayAnimation(enAnimationClip_Pull);
		break;
	default:
		break;
	}
}

void Lever::ProcessTransitionPushIdleState()
{
	//押すアニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//ドアのオブジェクトの配列を取得する。
		auto doors = FindGOs<Door>("door");
		//for文で配列を回す。
		for (auto door : doors)
		{
			//レバーの番号とドアの番号が一致していたら。
			if (m_leverNumber == door->GetDoorNumber())
			{
				//ドアに開けることを通知する。
				door->NotifyOpen();
				m_leverState = enLeverState_Push_Idle;
				break;
			}
		}
	}
}

void Lever::ProcessTransitionPullIdleState()
{
	//引くアニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//ドアのオブジェクトの配列を取得する。
		auto doors = FindGOs<Door>("door");
		//for文で配列を回す。
		for (auto door : doors)
		{
			//レバーの番号とドアの番号が一致していたら。
			if (m_leverNumber == door->GetDoorNumber())
			{
				//ドアに閉めることを通知する。
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
	//待機ステートの時。
	case enLeverState_Idle:
		//待機ステートのステート遷移処理。
		ProcessIdleStateTransition();
		break;
	//押すステートの時。
	case enLeverState_Push:
		//押すステートのステート遷移処理。
		ProcessPushStateTransition();
		break;
	//押し終わったステートの時。
	case enLeverState_Push_Idle:
		//押し終わったステートのステート遷移処理。
		ProcessPushIdleStateTransition();
		break;
	//引くステートの時。
	case enLeverState_Pull:
		//引くステートのステート遷移処理。
		ProcessPullStateTransition();
		break;
	//引き終わったステートの時。
	case enLeverState_Pull_Idle:
		//引き終わったステートのステート遷移処理。
		ProcessPullIdleStateTransition();
		break;
	default:
		break;
	}
}

void Lever::Render(RenderContext& rc)
{
	//モデルを描画する。
	m_modelRender.Draw(rc);
}