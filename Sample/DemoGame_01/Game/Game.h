#pragma once




//�N���X�錾�B
class Player;
class GameCamera;
class Background;
class Lever;
class Door;
class Fade;

/// <summary>
/// �Q�[���B
/// </summary>
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
	/// <summary>
	/// �Q�[���N���A�[��ʒm����B
	/// </summary>
	void NotifyGameClear();
	/// <summary>
	/// �Q�[���I�[�o�[��ʒm����B
	/// </summary>
	void NotifyGameOver();
	/// <summary>
	/// �G�l�~�[��S�ł������H
	/// </summary>
	/// <returns>�G�l�~�[���S�ł��Ă�����true�B</returns>
	const bool IsWannihilationEnemy() const
	{
		return m_numEnemy == m_numDefeatedEnemy;
	}
	/// <summary>
	/// �|���ꂽ�G�̐���+1����B
	/// </summary>
	void AddDefeatedEnemyNumber()
	{
		m_numDefeatedEnemy++;
	}
	//////////////////////////////////////
	// �����o�ϐ��B
	//////////////////////////////////////
private:
	LevelRender				m_levelRender;					//���x���B
	Player*					m_player = nullptr;				//�v���C���[�B
	GameCamera*				m_gameCamera = nullptr;			//�J�����B
	SkyCube*				m_skyCube = nullptr;			//��B
	Background*				m_background = nullptr;			//�w�i�B
	std::vector<Lever*>		m_leverVector;					//���o�[�̔z��B
	std::vector<Door*>		m_doorVector;					//�h�A�̔z��B
	SoundSource*			m_bgm = nullptr;				//�Q�[������BGM�B
	Fade*					m_fade = nullptr;				//�t�F�[�h�B
	bool					m_isWaitFadeout = false;
	int						m_numEnemy = 0;					//�G�l�~�[�̐��B
	int						m_numDefeatedEnemy = 0;			//�|���ꂽ�G�l�~�[�̐��B
};

