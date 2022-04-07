#pragma once

//�΂˃J�������g�p�������ꍇ�́ASpringCamera���C���N���[�h����B
#include "camera/SpringCamera.h"

//�N���X�錾�B
class Dancer;

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
	Dancer* m_dancer = nullptr;		//�v���C���[�B
	Vector3 m_toCameraPos = Vector3::One;
	SpringCamera m_springCamera;	//�΂˃J�����B
};

