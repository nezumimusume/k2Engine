#include "stdafx.h"
#include "Title.h"

#include "Fade.h"
#include "Game.h"

#include "sound/SoundSource.h"
#include "sound/SoundEngine.h"

namespace
{
	const float TITLE_TEXTURE_SCALE = 2.11f;
	const float PRESS_A_BUTTON_TEXTURE_SCALE = 1.0f;
	const Vector3 PRESS_A_BUTTON_TEXTURE_POSITION = Vector3(0.0f, -400.0f, 0.0f);
}

Title::Title()
{

}

Title::~Title()
{
	DeleteGO(m_bgm);
}

bool Title::Start()
{
	m_spriteRender.Init("Assets/sprite/title.dds", 1024, 512);
	//ëÂÇ´Ç≥Çê›íËÅB
	m_spriteRender.SetScale(Vector3(TITLE_TEXTURE_SCALE, TITLE_TEXTURE_SCALE, 1.0f));

	m_pressA.Init("Assets/sprite/press_a_button.dds", 1024, 256);
	m_pressA.SetScale(Vector3(PRESS_A_BUTTON_TEXTURE_SCALE, PRESS_A_BUTTON_TEXTURE_SCALE, 1.0f));
	m_pressA.SetPosition(PRESS_A_BUTTON_TEXTURE_POSITION);

	m_fade = FindGO<Fade>("fade");
	
	m_fade->StartFadeIn();

	g_soundEngine->ResistWaveFileBank(0, "Assets/sound/title.wav");
	g_soundEngine->ResistWaveFileBank(1, "Assets/sound/press_key.wav");

	m_bgm = NewGO<SoundSource>(0);
	m_bgm->Init(0);
	m_bgm->Play(true);
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
			SoundSource* se = NewGO<SoundSource>(0);
			se->Init(1);
			se->Play(false);
		}
	}

	m_spriteRender.Update();
	m_pressA.Update();
}

void Title::Render(RenderContext& rc)
{
	m_spriteRender.Draw(rc);
	m_pressA.Draw(rc);
}