#pragma once

//�΂˃J�������g�p�������ꍇ�́ASpringCamera���C���N���[�h����B
#include "camera/SpringCamera.h"

//�N���X�錾�B
class Player;

class GameCamera : public IGameObject
{
public:
	GameCamera();
	~GameCamera();
	bool Start();
	void Update();

	/////////////////////////////////////
	//�����o�ϐ�
	/////////////////////////////////////
	Player* m_player = nullptr;		//�v���C���[�B
	Vector3 m_toCameraPos = Vector3::One;
	SpringCamera m_springCamera;	//�΂˃J�����B
};

