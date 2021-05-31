#include "stdafx.h"
#include "Title.h"

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
	return true;
}

void Title::Update()
{
	m_spriteRender.Update();
}

void Title::Render(RenderContext& rc)
{
	m_spriteRender.Draw(rc);
}