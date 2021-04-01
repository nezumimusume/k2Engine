#pragma once

#include "Sound/SoundSource.h"
#include "Sound/SoundEngine.h"
#include "Sound/WaveFileBank.h"

class Player : public IGameObject
{
	bool Start() override
 	{
		m_modelRender.Init("Assets/modelData/unityChan.tkm");
		GetSoundEngine().GetWaveFileBank().Resist(0, "Assets/sound/sample.wav");
		return true;
	}

	void Update() override
	{
		if (g_pad[0]->IsTrigger(enButtonA))
		{
			SoundSource* se = NewGO<SoundSource>(0);
			se->Init(0, false);
			se->Play(false);
		}
		m_modelRender.Update();
	}

	void Render(RenderContext& rc) override
	{
		m_modelRender.Draw(rc);
	}
private:
	ModelRender m_modelRender;
};
