#include "stdafx.h"
#include "Game.h"

#include "Player.h"
#include "GameCamera.h"
#include "Box.h"
Game::Game()
{
	//�폜�B
	DeleteGO(m_player);
	DeleteGO(m_gameCamera);
	for (auto box : m_boxs)
	{
		DeleteGO(box);
	}
}

Game::~Game()
{

}

bool Game::Start()
{
	g_camera3D->SetPosition({ 0.0f, 100.0f, -600.0f });


	m_gameCamera = NewGO<GameCamera>(0, "gameCamera");

	//���x�����\�z����B
	m_levelRender.Init("Assets/level/sample.tkl", [&](LevelObjectData& objData) {
		if (objData.EqualObjectName(L"sample") == true) {

			auto box = NewGO<Box>(0);
			//�z�u���W�A�X�P�[���A��]���擾����B
			box->m_position = objData.position;
			box->m_scale = objData.scale;
			box->m_rotation = objData.rotation;
			//true�ɂ���ƁA���x���̕��Ń��f�����ǂݍ��܂�Ȃ��B
			return true;
		}
		else if (objData.EqualObjectName(L"player") == true) {
			//Unity�����B
			//�v���C���[�̃C���X�^���X�𐶐�����B
			m_player = NewGO<Player>(0, "player");
			m_player->m_position = objData.position;
			//true�ɂ���ƁA���x���̕��Ń��f�����ǂݍ��܂�Ȃ��B
			return true;
		}
		if (objData.EqualObjectName(L"bg") == true) {
			//false�ɂ���ƁA���x���̕��Ń��f�����ǂݍ��܂�Ĕz�u�����B
			return false;
		}
		return false;
	});

	return true;
}

void Game::Update()
{

}

void Game::Render(RenderContext& rc)
{
	//���x���œǂݍ��񂾃��f����\��������B
	m_levelRender.Draw(rc);
}
