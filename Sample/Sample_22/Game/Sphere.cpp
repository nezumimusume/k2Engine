#include "stdafx.h"
#include "Sphere.h"
#include "SphereRender.h"

Sphere::Sphere()
{

}

Sphere::~Sphere()
{

}

bool Sphere::Start()
{
	m_sphereRender = FindGO<SphereRender>("sphererender");

	return true;
}

void Sphere::Update()
{
	m_sphereRender->UpdateInstancingData(
		m_position,
		m_rotation,
		m_scale
	);
}