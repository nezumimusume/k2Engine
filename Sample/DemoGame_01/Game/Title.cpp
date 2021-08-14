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
	//BGM���폜����B
	DeleteGO(m_bgm);
}

bool Title::Start()
{
	//�摜��ǂݍ��ށB
	m_spriteRender.Init("Assets/sprite/title.dds", 2211,1105);
	m_pressButton.Init("Assets/sprite/button.dds", 1293, 646);
	m_pressButton.SetPosition(Vector3(0.0f, -220.0f, 0.0f));

	//����ǂݍ��ށB
	g_soundEngine->ResistWaveFileBank(8, "Assets/sound/titlebgm.wav");
	g_soundEngine->ResistWaveFileBank(9, "Assets/sound/button.wav");

	//BGM�B
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
			//���g���폜����B
			DeleteGO(this);
		}
	}
	else {
		//A�{�^������������B
		if (g_pad[0]->IsTrigger(enButtonA)) {
			m_isWaitFadeout = true;
			m_fade->StartFadeOut();
			//���ʉ����Đ�����B
			SoundSource* se = NewGO<SoundSource>(0);
			se->Init(9);
			se->Play(false);
			se->SetVolume(0.6f);
		}
	}

	//���l��ω�������B
	if (m_isWaitFadeout)
	{
		m_alpha += g_gameTime->GetFrameDeltaTime() * 20.5f;
	}
	else
	{
		m_alpha += g_gameTime->GetFrameDeltaTime() * 1.2f;
	}

	m_pressButton.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, fabsf(sinf(m_alpha))));

	//�摜�̍X�V�B
	m_spriteRender.Update();
	m_pressButton.Update();
}

void Title::Render(RenderContext& rc)
{
	//�摜�̕`��B
	m_spriteRender.Draw(rc);
	m_pressButton.Draw(rc);
}