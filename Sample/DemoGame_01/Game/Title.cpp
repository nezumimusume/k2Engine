#include "stdafx.h"
#include "Title.h"

#include "Game.h"

Title::Title()
{

}

Title::~Title()
{

}

bool Title::Start()
{
	m_spriteRender.Init("Assets/sprite/title.dds", 1457,728);
	m_pressButton.Init("Assets/sprite/button.dds", 931, 465);
	m_pressButton.SetPosition(Vector3(0.0f, -130.0f, 0.0f));
	return true;
}

void Title::Update()
{
	if (g_pad[0]->IsTrigger(enButtonA))
	{
		NewGO<Game>(0, "game");
		DeleteGO(this);
	}

	m_alpha += g_gameTime->GetFrameDeltaTime() * 1.2f;
	m_pressButton.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, fabsf(sinf(m_alpha))));

	m_spriteRender.Update();
	m_pressButton.Update();
}

void Title::Render(RenderContext& rc)
{
	m_spriteRender.Draw(rc);
	m_pressButton.Draw(rc);
	
}