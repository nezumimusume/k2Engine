#pragma once

#include "Sound/SoundSource.h"
#include "Sound/SoundEngine.h"
#include "Sound/WaveFileBank.h"

class Player : public IGameObject
{
	bool Start() override
 	{
		m_modelRender.Init("Assets/modelData/unityChan.tkm");
		SoundEngine::GetInstance().GetWaveFileBank().Resist(0, "Assets/sound/sample.wav");
		SoundEngine::GetInstance().GetWaveFileBank().Resist(1, "Assets/sound/shining_star.wav");
		SoundEngine::GetInstance().GetWaveFileBank().Resist(2, "Assets/sound/sample2.wav");

		m_bgm = NewGO<SoundSource>(0);
		m_bgm->Init(1, false);
		m_bgm->Play(true);
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

		if (g_pad[0]->IsTrigger(enButtonB))
		{
			//’âŽ~’†B
			if (m_isStop)
			{
				m_bgm->Play(true);
				m_isStop = false;
			}
			else {
				m_bgm->Pause();
				m_isStop = true;
			}
		}
		m_modelRender.Update();
	}

	void Render(RenderContext& rc) override
	{
		m_modelRender.Draw(rc);
	}
private:
	ModelRender m_modelRender;
	SoundSource* m_bgm = nullptr;
	bool m_isStop = false;
};
