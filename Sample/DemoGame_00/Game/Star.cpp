#include "stdafx.h"
#include "Star.h"

#include "Player.h"
#include "Game.h"

#include "sound/SoundSource.h"
#include "sound/SoundEngine.h"

bool Star::Start()
{
	m_modelRender.Init("Assets/modelData/star.tkm");
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetScale(m_scale);

	m_player = FindGO<Player>("player");

	g_soundEngine->ResistWaveFileBank(5, "Assets/sound/coinGet.wav");
	return true;
}

void Star::Update()
{
	if (!m_isGet) {
		Vector3 dist = m_player->GetPosition() - m_position;
		if (dist.Length() < 130.0f) {
			FindGO<Game>("game")->AddGetStarCount();
			m_jumpSpeed = 500.0f;
			m_isGet = true;
			m_jumStartPosY = m_position.y;
			SoundSource* se = NewGO<SoundSource>(0);
			se->Init(5);
			se->Play(false);
		}
	}
	else {

		m_position.y += m_jumpSpeed * g_gameTime->GetFrameDeltaTime();
		m_jumpSpeed -= 980.0f * g_gameTime->GetFrameDeltaTime();
		Quaternion qAddRot;
		qAddRot.SetRotation(Vector3::AxisY, 0.2f);
		m_rotation.Multiply(qAddRot);

		if (m_jumStartPosY > m_position.y) {
			DeleteGO(this);
		}
	}
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);

	m_modelRender.Update();
}

void Star::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}
