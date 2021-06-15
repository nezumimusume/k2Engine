#include "stdafx.h"
#include "FireBall.h"

#include "graphics/effect/EffectEmitter.h"

FireBall::~FireBall()
{
	m_effectEmitter->Stop();
	DeleteGO(m_effectEmitter);
}

bool FireBall::Start()
{
	EffectEngine::GetInstance()->ResistEffect(0, u"Assets/effect/efk/magic_fire.efk");


	m_effectEmitter = NewGO <EffectEmitter>(0);
	m_effectEmitter->Init(0);
	m_position.y += 80.0f;
	m_effectEmitter->SetPosition(m_position);
	m_effectEmitter->SetScale(Vector3::One * 12.0f);
	m_moveSpeed = Vector3::AxisZ;
	m_rotation.Apply(m_moveSpeed);
	m_rotation.AddRotationDegY(360.0f);
	m_effectEmitter->SetRotation(m_rotation);
	m_moveSpeed *= 1000.0f;
	m_effectEmitter->Play();

	return true;
}

void FireBall::Update()
{
	m_position += m_moveSpeed * g_gameTime->GetFrameDeltaTime();
	m_effectEmitter->SetPosition(m_position);

	m_timer += g_gameTime->GetFrameDeltaTime();

	if (m_timer >= 1.0f)
	{
		DeleteGO(this);
	}
}