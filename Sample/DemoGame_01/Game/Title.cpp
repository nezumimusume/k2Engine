#include "stdafx.h"
#include "Title.h"

#include "Game.h"
#include "Fade.h"

#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"

Title::Title()
{

}

Title::~Title()
{
	DeleteGO(m_bgm);
}

bool Title::Start()
{
	m_spriteRender.Init("Assets/sprite/title.dds", 1457,728);
	m_pressButton.Init("Assets/sprite/button.dds", 931, 465);
	m_pressButton.SetPosition(Vector3(0.0f, -130.0f, 0.0f));

	g_soundEngine->ResistWaveFileBank(8, "Assets/sound/titlebgm.wav");
	m_bgm = NewGO<SoundSource>(0);
	m_bgm->Init(8);
	m_bgm->Play(true);
	m_bgm->SetVolume(0.3f);

	m_fade = FindGO<Fade>("fade");
	m_fade->StartFadeIn();
	return true;
}

void Title::Update()
{
	if (m_isWaitFadeout) {
		if (!m_fade->IsFade()) {
			NewGO<Game>(0, "game");
			DeleteGO(this);
		}
	}
	else {
		if (g_pad[0]->IsTrigger(enButtonA)) {
			m_isWaitFadeout = true;
			m_fade->StartFadeOut();
		}
	}

	if (m_isWaitFadeout)
	{
		m_alpha += g_gameTime->GetFrameDeltaTime() * 20.5f;
	}
	else
	{
		m_alpha += g_gameTime->GetFrameDeltaTime() * 1.2f;
	}

	m_pressButton.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, fabsf(sinf(m_alpha))));

	m_spriteRender.Update();
	m_pressButton.Update();
}

void Title::Render(RenderContext& rc)
{
	m_spriteRender.Draw(rc);
	m_pressButton.Draw(rc);
	
}