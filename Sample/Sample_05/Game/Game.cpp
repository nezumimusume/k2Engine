#include "stdafx.h"
#include "Game.h"

#include "Player.h"
#include "GameCamera.h"
#include "Box.h"

Game::Game()
{
	
}

Game::~Game()
{
	//�폜�B
	DeleteGO(m_player);
	DeleteGO(m_gameCamera);
	for (auto box : m_boxs)
	{
		DeleteGO(box);
	}
	for (auto pointLight : m_pointLightList)
	{
		//�|�C���g���C�g���폜����B
		delete pointLight;
	}
}

bool Game::Start()
{
	g_camera3D->SetPosition({ 0.0f, 100.0f, 800.0f });


	m_gameCamera = NewGO<GameCamera>(0, "gameCamera");

	//���x�����\�z����B
	m_levelRender.Init("Assets/level/sample.tkl", [&](LevelObjectData& objData) {
		if (objData.EqualObjectName(L"sample") == true) {

			auto box = NewGO<Box>(0);
			//�z�u���W�A�X�P�[���A��]���擾����B
			box->m_position = objData.position;
			box->m_scale = objData.scale;
			box->m_rotation = objData.rotation;
			
			//�|�C���g���C�g���쐬����B
			PointLight* pointLight = new PointLight;
			pointLight->Init();
			Vector3 pointLightPosition = objData.position;
			pointLightPosition.y += 100.0f;
			//�|�C���g���C�g�̍��W��ݒ肷��B
			pointLight->SetPosition(pointLightPosition);
			m_pointLightList.push_back(pointLight);
			//true�ɂ���ƁA���x���̕��Ń��f�����ǂݍ��܂�Ȃ��B
			return true;
		}
		else if (objData.EqualObjectName(L"player") == true) {
			//Unity�����B
			//�v���C���[�̃C���X�^���X�𐶐�����B
			m_player = NewGO<Player>(0, "player");
			m_player->m_position = objData.position;
			m_player->m_scale = objData.scale;
			//true�ɂ���ƁA���x���̕��Ń��f�����ǂݍ��܂�Ȃ��B
			return true;
		}
		if (objData.EqualObjectName(L"bg") == true) {
			//false�ɂ���ƁA���x���̕��Ń��f�����ǂݍ��܂�Ĕz�u�����B
			return false;
		}
		return false;
	});


	//�|�C���g���C�g�̃p�����[�^��ݒ肷��B
	//�|�C���g���C�g�̐F�B
	m_pointLightList[0]->SetColor(Vector3(40.0f, 5.0f, 5.0f));
	//�|�C���g���C�g�̉e���͈́B
	m_pointLightList[0]->SetRange(250.0f);
	//�|�C���g���C�g�̌������B
	m_pointLightList[0]->SetAffectPowParam(1.0f);

	m_pointLightList[1]->SetColor(Vector3(5.0f, 20.0f, 5.0f));
	m_pointLightList[1]->SetRange(350.0f);
	m_pointLightList[1]->SetAffectPowParam(3.0f);

	m_pointLightList[2]->SetColor(Vector3(5.0f, 5.0f, 20.0f));
	m_pointLightList[2]->SetRange(500.0f);
	m_pointLightList[2]->SetAffectPowParam(2.0f);
	return true;
}

void Game::Update()
{
	for (auto pt : m_pointLightList) {
		pt->Update();
	}
}

void Game::Render(RenderContext& rc)
{
	//���x���œǂݍ��񂾃��f����\��������B
	m_levelRender.Draw(rc);
}
