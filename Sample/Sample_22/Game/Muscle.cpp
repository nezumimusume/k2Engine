#include "stdafx.h"
#include "Muscle.h"
#include "MuscleRender.h"

Muscle::Muscle()
{

}

Muscle::~Muscle()
{

}

bool Muscle::Start()
{
	m_muscleRender = FindGO<MuscleRender>("musclerender");

	return true;
}

void Muscle::Update()
{
	m_muscleRender->UpdateInstancingData(
		m_position,
		m_rotation,
		m_scale
	);
}