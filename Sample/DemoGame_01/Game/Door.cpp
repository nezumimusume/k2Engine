#include "stdafx.h"
#include "Door.h"

#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"

Door::Door()
{

}

Door::~Door()
{

}

bool Door::Start()
{
	//アニメーションを読み込む。
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/door/idle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Open].Load("Assets/animData/door/open.tka");
	m_animationClips[enAnimationClip_Open].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Close].Load("Assets/animData/door/close.tka");
	m_animationClips[enAnimationClip_Close].SetLoopFlag(false);
	//モデルを読み込む。
	m_modelRender.Init("Assets/modelData/door/door.tkm", m_animationClips, enAnimationClip_Num);
	//座標を設定する。
	m_modelRender.SetPosition(m_position);
	//大きさを設定する。
	m_modelRender.SetScale(m_scale);
	//回転を設定する。
	m_modelRender.SetRotation(m_rotation);

	//モデルの更新。
	m_modelRender.Update();
	//当たり判定を作成する。
	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());

	//音を読み込む。
	g_soundEngine->ResistWaveFileBank(5, "Assets/sound/door_cut.wav");
	return true;
}

void Door::Update()
{
	//アニメーションを再生する。
	PlayAnimation();
	//ステート管理。
	ManageState();

	//モデルの更新。
	m_modelRender.Update();
}

void Door::NotifyOpen()
{
	//オープンステートに遷移する。
	m_doorState = enDoorState_Open;
	//音を再生する。
	SoundSource* se = NewGO<SoundSource>(0);
	se->Init(5);
	se->Play(false);
	se->SetVolume(0.7f);
}

void Door::NotifyClose()
{
	//クローズステートに遷移する。
	m_doorState = enDoorState_Close;
	//音を再生する。
	SoundSource* se = NewGO<SoundSource>(0);
	se->Init(5);
	se->Play(false);
	se->SetVolume(0.6f);
}

void Door::PlayAnimation()
{
	switch (m_doorState)
	{
	//待機ステートの時。
	case enDoorState_Idle:
		//待機アニメーションを再生する。
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;
	//オープンステートの時。
	case enDoorState_Open:
		//オープンアニメーションを再生する。
		m_modelRender.PlayAnimation(enAnimationClip_Open);
		m_modelRender.SetAnimationSpeed(0.6f);
		break;
	//クローズステートの時。
	case enDoorState_Close:
		//クローズアニメーションを再生する。
		m_modelRender.PlayAnimation(enAnimationClip_Close);
		m_modelRender.SetAnimationSpeed(0.6f);
		break;
	default:
		break;
	}
}

void Door::ReleasePhysicsObject()
{
	//当たり判定を開放する。
	m_physicsStaticObject.Release();
}

void Door::CreatePhysicsObject()
{
	//当たり判定を作成する。
	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());
}

void Door::ProcessOpenStateTransition()
{
	//オープンアニメーションの再生が終了したら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//当たり判定を開放する。
		ReleasePhysicsObject();
		//オープン終わりステートに遷移する。
		m_doorState = enDoorState_Open_Idle;
	}
}

void Door::ProcessCloseStateTransition()
{
	//クローズアニメーションが終了したら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//当たり判定を作成する。
		CreatePhysicsObject();
		//クローズ終わりステートに遷移する。
		m_doorState = enDoorState_Close_Idle;
	}
}

void Door::ManageState()
{
	switch (m_doorState)
	{
	//待機ステートの時。
	case Door::enDoorState_Idle:
		
		break;
	//オープンステートの時。
	case Door::enDoorState_Open:
		//オープンステートのステート遷移処理。
		ProcessOpenStateTransition();
		break;
	//オープン終わりステートの時。
	case Door::enDoorState_Open_Idle:
		
		break;
	//クローズステートの時。
	case Door::enDoorState_Close:
		//クローズステートのステート遷移処理。
		ProcessCloseStateTransition();
		break;
	//クローズ終わりステートの時。
	case Door::enDoorState_Close_Idle:
		
		break;
	default:
		break;
	}
}

void Door::Render(RenderContext& rc)
{
	//モデルを描画する。
	m_modelRender.Draw(rc);
}

