#include "stdafx.h"
#include "FireBall.h"

#include "collision/CollisionObject.h"
#include "graphics/effect/EffectEmitter.h"
#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"

FireBall::FireBall()
{

}

FireBall::~FireBall()
{
	m_effectEmitter->Stop();
	DeleteGO(m_effectEmitter);
	DeleteGO(m_collisionObject);
}

bool FireBall::Start()
{
	EffectEngine::GetInstance()->ResistEffect(0, u"Assets/effect/efk/magic_fire.efk");

	m_effectEmitter = NewGO <EffectEmitter>(0);
	m_effectEmitter->Init(0);
	
	m_effectEmitter->SetScale(Vector3::One * 20.0f);
	m_moveSpeed = Vector3::AxisZ;
	m_rotation.Apply(m_moveSpeed);
	m_position += m_moveSpeed * 50.0f;
	m_moveSpeed *= 2000.0f;
	m_rotation.AddRotationDegY(360.0f);
	m_effectEmitter->SetRotation(m_rotation);
	m_effectEmitter->Play();

	m_collisionObject = NewGO<CollisionObject>(0);
	m_collisionObject->CreateSphere(m_position, Quaternion::Identity, 40.0f);
	m_collisionObject->SetName("fire");
	m_collisionObject->SetIsEnableAutoDelete(false);
	// step-1 
	g_soundEngine->ResistWaveFileBank(1, "Assets/sound/fire.wav");
	SoundSource* se = NewGO<SoundSource>(0);
	se->Init(1);
	se->Play(false);
	se->SetVolume(0.7f);
	return true;
}

void FireBall::Update()
{
	m_position += m_moveSpeed * g_gameTime->GetFrameDeltaTime();
	m_effectEmitter->SetPosition(m_position);
	m_collisionObject->SetPosition(m_position);

	m_timer += g_gameTime->GetFrameDeltaTime();

	if (m_timer >= 0.4f)
	{
		DeleteGO(this);
	}
}