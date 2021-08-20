#pragma once
#include "level3DRender/LevelRender.h"

//�N���X�錾�B
class Player;
class GameCamera;
class BackGround;
class Sphere;

class Game : public IGameObject
{
public:
	
public:
	//////////////////////////////////////
	// �����o�֐��B
	//////////////////////////////////////
	Game();
	~Game();
	bool Start();
	void Update();
private:
	/// <summary>
	/// ����������B
	/// </summary>
	void InitSky();
private:
	Player*					m_player = nullptr;			//�v���C���[�B
	GameCamera*				m_gameCamera = nullptr;		//�Q�[���J�����B
	SkyCube*				m_skyCube = nullptr;		//�X�J�C�L���[�u�B
	int						m_skyCubeType = enSkyCubeType_Day;
	BackGround*				m_backGround = nullptr;
	Sphere*					m_sphere = nullptr;
};

