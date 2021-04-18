#pragma once

#include "Sound/SoundSource.h"
#include "Sound/SoundEngine.h"

class Player : public IGameObject
{
public:
	bool Start() override
 	{
		m_modelRender.Init("Assets/modelData/unityChan.tkm");
		g_soundEngine->ResistWaveFileBank(0, "Assets/sound/sample.wav");
		g_soundEngine->ResistWaveFileBank(1, "Assets/sound/shining_star.wav");
		g_soundEngine->ResistWaveFileBank(2, "Assets/sound/sample2.wav");

		//m_bgm = NewGO<SoundSource>(0);
		//m_bgm->Init(1, false);
		//m_bgm->Play(true);

		//PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();
		m_modelRender.Update();
		m_po.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());

		sR.Init("Assets/Image/sample.dds", 100.0f, 100.0f);

		fontRender.SetText(L"aiueo");
		fontRender.SetScale(5.0f);
		fontRender.SetPosition({ 00.0f,300.0f,00.0f });
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
		m_modelRender.SetScale(m_scale);
		m_modelRender.SetPosition(m_position);
		m_modelRender.Update();
		sR.Update();
		auto playerVector = FindGOs<Player>("player");
		int a = 0;
		for (auto player : playerVector)
		{
			a++;
		}
	
	}

	void Render(RenderContext& rc) override
	{
		m_modelRender.Draw();
		sR.Draw(rc);
		fontRender.Draw(rc);
	}

	ModelRender m_modelRender;
	SoundSource* m_bgm = nullptr;
	bool m_isStop = false;
	PhysicsStaticObject m_po;
	SpriteRender sR;
	FontRender fontRender;
	Vector3 m_position;
	Vector3 m_scale = Vector3::One;
};
