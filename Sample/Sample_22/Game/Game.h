#pragma once

//���x�������_�[���g�p����ɂ�LevelRender���C���N���[�h����K�v������B
#include "level3DRender/LevelRender.h"

//�N���X�錾�B
class Player;
class GameCamera;
class Sphere;
class SphereRender;
class Muscle;
class MuscleRender;

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
	std::vector<Sphere*> m_spheres;				//�e�B
	SphereRender* m_sphereRender = nullptr;
	std::vector<Muscle*> m_muscles;				//���B
	MuscleRender* m_muscleRender = nullptr;
	int m_maxSphere = 0;
	int m_maxMuscle = 0;
	bool m_isDrawInstancing = true;
};

