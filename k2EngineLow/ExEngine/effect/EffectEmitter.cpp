#include "k2EngineLowPreCompile.h"
#include "EffectEmitter.h"

EffectEmitter::EffectEmitter()
{

}

EffectEmitter::~EffectEmitter()
{

}

void EffectEmitter::Init(const int number)
{
	m_effect.Init(number);
}

void EffectEmitter::Update()
{
	m_effect.Update();

	if (!IsPlay())
	{
		DeleteGO(this);
	}
}