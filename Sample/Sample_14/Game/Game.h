#pragma once

//���x�������_�[���g�p����ɂ�LevelRender���C���N���[�h����K�v������B
#include "level3DRender/LevelRender.h"

//�N���X�錾�B
class Player;
class GameCamera;
class BoxMove;
class BoxRotation;

class Game : public IGameObject
{
public:
	//////////////////////////////////////
	// �����o�֐��B
	//////////////////////////////////////
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	////////////////////////////////////
	// �����o�ϐ��Ƃ�
	////////////////////////////////////
	Player* m_player = nullptr;				//�v���C���[�B
	GameCamera* m_gameCamera = nullptr;		//�Q�[���J�����B
	LevelRender m_levelRender;				//���x�������_�[�B

	std::vector<BoxMove*> m_boxmoves;				//move�{�b�N�X�B
	std::vector<BoxRotation*> m_boxrots;			//rotation�{�b�N�X�B
};

