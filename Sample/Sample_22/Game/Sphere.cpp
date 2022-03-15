#include "stdafx.h"
#include "Sphere.h"
#include "SphereRender.h"
#include "Game.h"

Sphere::Sphere()
{

}

Sphere::~Sphere()
{
	m_sphereRender->RemoveInstance(m_instanceNo);
}

bool Sphere::Start()
{
	m_sphereRender = FindGO<SphereRender>("sphererender");
	m_game = FindGO<Game>("game");
	m_modelRender.Init("Assets/modelData/sample1.tkm");
	m_modelRender.SetTRS(m_position, m_rotation, m_scale);
	m_modelRender.Update();
	return true;
}

void Sphere::Update()
{

	if (m_game->m_isDrawInstancing == true)
	{
		m_sphereRender->UpdateInstancingData(
			m_instanceNo,
			m_position,
			m_rotation,
			m_scale
		);

	}
	
}

void Sphere::Render(RenderContext& rc)
{
	if (m_game->m_isDrawInstancing == false)
	{
		m_modelRender.Draw(rc);
	}
}