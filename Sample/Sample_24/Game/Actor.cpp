#include "stdafx.h"
#include "Actor.h"

void Actor::Init(GamePad& gamePad, const char* filePath, const Vector3& initPos)
{
	m_gamePad = &gamePad;
	m_modelRender.Init(filePath);
	m_position = initPos;
	m_modelRender.SetPosition(initPos);
}
void Actor::Update()
{
	m_position.x += m_gamePad->GetLStickXF() * 4.0f;
	m_position.y += m_gamePad->GetLStickYF() * 4.0f;
	m_modelRender.SetPosition(m_position);
	m_modelRender.Update();
}
void Actor::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}