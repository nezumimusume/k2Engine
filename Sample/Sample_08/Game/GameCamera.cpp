#include "stdafx.h"
#include "GameCamera.h"

#include "Player.h"

GameCamera::GameCamera()
{

}

GameCamera::~GameCamera()
{

}

bool GameCamera::Start()
{
	//�v���C���[�̃C���X�^���X��T���B
	m_player = FindGO<Player>("player");

	return true;
}

void GameCamera::Update()
{
	//�J�������X�V�B
	//�����_���v�Z����B
	Vector3 target = m_player->m_position;
	//�v���C���̑������炿����Ə�𒍎��_�Ƃ���B
	target.y += 100.0f;
	//���_���v�Z����B
	//�L�����N�^���΂ߏォ�猩�Ă���悤�Ȏ��_�ɂ���B
	Vector3 toPos = { 0.0f, 50.0f, -300.0f };
	Vector3 pos = target + toPos;	
	//���C���J�����ɒ����_�Ǝ��_��ݒ肷��B
	g_camera3D->SetTarget(target);
	g_camera3D->SetPosition(pos);

	//�J�����̍X�V�B
	g_camera3D->Update();
}