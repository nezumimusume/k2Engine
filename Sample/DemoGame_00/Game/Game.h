#pragma once
#include "level3DRender/LevelRender.h"

//�N���X�錾�B
class Player;
class Background;
class GameCamera;
class Fade;
class Enemy;

class Game : public IGameObject
{
public:
	enum EnGameState
	{
		enGameState_DuringGamePlay,			//�Q�[���v���C���B
		enGameState_GameOver,				//�Q�[���I�[�o�[�B
		enGameState_GameClear,				//�Q�[���N���A�B
		enGameState_GameClear_Idle			//�Q�[���N���A(�ҋ@��)�B
	};
public:
	//////////////////////////////////////
	// �����o�֐��B
	//////////////////////////////////////
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	/// <summary>
	/// �l���������̐���+1����B
	/// </summary>
	void AddGetStarCount()
	{
		m_starCount++;
	}
	/// <summary>
	/// �Q�[���I�[�o�[��ʒm����B
	/// </summary>
	void NotifyGameOver();
	/// <summary>
	/// �Q�[���N���A��ʒm����B
	/// </summary>
	void NotifyGameClear();
	void NotifyGameClearIdle();
	/// <summary>
	/// ���X�^�[�g��ʒm����B
	/// </summary>
	void NotifyReStart();
private:
	/// <summary>
	/// �t�H���g�̍X�V�B
	/// </summary>
	void UpdateFont();
	/// <summary>
	/// �^�C�}�[�����Z����B
	/// </summary>
	void CountTimer();
	/// <summary>
	/// ����������B
	/// </summary>
	void InitSky();
private:
	Player*					m_player = nullptr;			//�v���C���[�B
	Background*				m_background = nullptr;		//�w�i�B
	GameCamera*				m_gameCamera = nullptr;		//�Q�[���J�����B
	std::vector<Enemy*>		m_enemys;			//�G�l�~�[�̃��X�g�B
	LevelRender				m_levelRender;				//���x���B
	SkyCube*				m_skyCube = nullptr;		//�X�J�C�L���[�u�B
	bool					m_isWaitFadeout = false;
	Fade*					m_fade = nullptr;			//�t�F�[�h�B
	int						m_starCount = 0;			//�l���������̐��B
	FontRender				m_starCountFont;			//�l���������̐���\������t�H���g�B
	Vector3					m_starCountFontPosition;
	float					m_timer = 0.0f;				//�^�C�}�[�B
	FontRender				m_timerFont;				//�^�C�}�[��\������t�H���g�B
	EnGameState				m_gameState = enGameState_DuringGamePlay;		//�Q�[���X�e�[�g�B
	SpriteRender			m_pressA;
	std::vector<PointLight*> m_pointLightList;
	std::vector<SpotLight*> m_spotLightList;
	std::vector<VolumeSpotLight*> m_volumeSpotLightList;
	int m_skyCubeType = enSkyCubeType_Day;
	SoundSource* m_bgm = nullptr;
};

