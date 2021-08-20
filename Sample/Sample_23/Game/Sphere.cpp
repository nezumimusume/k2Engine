#include "stdafx.h"
#include "Sphere.h"

bool Sphere::Start()
{
	m_modelRender.Init("Assets/modelData/sphere/sphere.tkm");
	m_modelRender.SetPosition(Vector3(80.0f, 50.0f, 0.0f));
	return true;
}

void Sphere::Update()
{
	m_modelRender.Update();
}

void Sphere::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}