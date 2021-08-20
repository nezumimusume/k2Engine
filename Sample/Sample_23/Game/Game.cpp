#include "stdafx.h"
#include "Game.h"

//�X�J�C�L���[�u�B����B
#include "nature/SkyCube.h"

#include "Player.h"
#include "GameCamera.h"
#include "Sphere.h"

#include "graphics/RenderingEngine.h"

Game::Game()
{
	
}

Game::~Game()
{
	//�폜�B
	DeleteGO(m_player);
	DeleteGO(m_gameCamera);
	DeleteGO(m_skyCube);
	DeleteGO(m_sphere);
}
void Game::InitSky()
{
	// ���݂̋��j���B
	DeleteGO(m_skyCube);
	m_skyCube = NewGO<SkyCube>(0, "skycube");

	m_skyCube->SetLuminance(1.0f);
	m_skyCube->SetType((EnSkyCubeType)m_skyCubeType);

	// �����̌v�Z�̂��߂�IBL�e�N�X�`�����Z�b�g����B
	g_sceneLight->SetIBLTextureForAmbient(m_skyCube->GetTextureFilePath(), 1.0f);
	// �������̉e����������₷���悤�ɁA�f�B���N�V�������C�g�̓I�t�ɁB
	g_sceneLight->SetDirectionLight(0, g_vec3Zero, g_vec3Zero);
}
bool Game::Start()
{
	
	
	InitSky();
	
	m_gameCamera = NewGO<GameCamera>(0, "gameCamera");
	m_player = NewGO<Player>(0, "player");
	m_sphere = NewGO<Sphere>(0, "sphere");

	return true;
}


void Game::Update()
{
	//�����ύX���Ă�B
	if (g_pad[0]->IsTrigger(enButtonRight)) {
		m_skyCubeType++;
		m_skyCubeType = m_skyCubeType % enSkyCubeType_Num;
		InitSky();
	}
	if (g_pad[0]->IsTrigger(enButtonLeft)) {
		m_skyCubeType--;
		if (m_skyCubeType < 0) {
			m_skyCubeType = enSkyCubeType_Num - 1;
		}
		InitSky();
	}
	
}
