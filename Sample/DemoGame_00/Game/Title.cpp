#include "stdafx.h"
#include "Title.h"

#include "Fade.h"
#include "Game.h"

Title::Title()
{

}

Title::~Title()
{

}

bool Title::Start()
{
	m_spriteRender.Init("Assets/sprite/title.dds", 1024, 512);
	//‘å‚«‚³‚ğİ’èB
	m_spriteRender.SetScale(Vector3(1.435f, 1.435f, 1.0f));

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

	m_spriteRender.Update();
}

void Title::Render(RenderContext& rc)
{
	m_spriteRender.Draw(rc);
}