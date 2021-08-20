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
private:
	Player*			m_player = nullptr;				//�v���C���[�B
	Vector3			m_toCameraPos = Vector3::One;	//�����_���王�_�Ɍ������x�N�g���B
};

