#include "stdafx.h"
#include "Game.h"

#include "Player.h"
#include "GameCamera.h"

#include "BoxMove.h"
#include "BoxRotation.h"

Game::Game()
{

}

Game::~Game()
{
	//�폜�B
	DeleteGO(m_player);
	DeleteGO(m_gameCamera);
	//�v�b�V�������{�b�N�X���폜���Ă����B
	for (auto box : m_boxmoves)
	{
		DeleteGO(box);
	}
	for (auto box : m_boxrots)
	{
		DeleteGO(box);
	}
}

bool Game::Start()
{
	g_camera3D->SetPosition({ 0.0f, 100.0f, -600.0f });

	m_gameCamera = NewGO<GameCamera>(0, "gameCamera");

	m_player = NewGO<Player>(0, "player");

	//���x�����\�z����B
	m_levelRender.Init("Assets/level/sample.tkl", [&](LevelObjectData& objData) {

		if (objData.EqualObjectName(L"Box_Move") == true) {

			auto box = NewGO<BoxMove>(0);
			//�z�u���W�A�X�P�[���A��]���擾����B
			box->m_position = objData.position;
			box->m_scale = objData.scale;
			box->m_rotation = objData.rotation;
			//��ō폜���邽�߂ɁA�{�b�N�X���v�b�V�����Ă����B
			m_boxmoves.push_back(box);

			//true�ɂ���ƁA���x���̕��Ń��f�����ǂݍ��܂�Ȃ��B
			return true;
		}

		if (objData.EqualObjectName(L"Box_Rotation") == true) {

			auto box = NewGO<BoxRotation>(0);
			//�z�u���W�A�X�P�[���A��]���擾����B
			box->m_position = objData.position;
			box->m_scale = objData.scale;
			box->m_rotation = objData.rotation;
			//��ō폜���邽�߂ɁA�{�b�N�X���v�b�V�����Ă����B
			m_boxrots.push_back(box);

			//true�ɂ���ƁA���x���̕��Ń��f�����ǂݍ��܂�Ȃ��B
			return true;
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
