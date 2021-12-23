#pragma once

//�N���X�錾�B
class MovedPath;
class Player;
class Game;

class Enemy : public IGameObject
{
public:
	enum EnEnemyState
	{
		enEnemyState_Idle,
		enEnemyState_Run,
		enEnemyState_Num
	};
public:
	////////////////////////////////////
	// �����o�֐�
	////////////////////////////////////
	Enemy();
	~Enemy();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	/// <summary>
	/// �p�X�f�[�^��ǂݍ��ށB
	/// </summary>
	/// <param name="number">�ԍ��B</param>
	void LoadPath(const int number);
	/// <summary>
	/// ���W��ݒ�B
	/// </summary>
	/// <param name="position">���W�B</param>
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	/// <summary>
	/// ���W���擾�B
	/// </summary>
	/// <returns>���W�B</returns>
	const Vector3& GetPosition() const
	{
		return m_position;
	}
	/// <summary>
	/// �傫����ݒ�B
	/// </summary>
	/// <param name="scale">�傫���B</param>
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}
private:
	/// <summary>
	/// �ړ������B
	/// </summary>
	void Move();
	/// <summary>
	/// ��]�����B
	/// </summary>
	void Turn();
	/// <summary>
	/// �X�e�[�g���Ǘ��B
	/// </summary>
	void ManageState();
	/// <summary>
	/// �A�j���[�V�����̍Đ��B
	/// </summary>
	void PlayAnimation();
	/// <summary>
	/// �v���C���[��T������B
	/// </summary>
	void SearchPlayer();
	/// <summary>
	/// �v���C���[�̃|�C���^���擾����B
	/// </summary>
	/// <returns>�v���C���[�̃|�C���^���擾�ς݂Ȃ�true�B</returns>
	bool GetPlayer()
	{
		if (m_player == nullptr)
		{
			m_player = FindGO<Player>("player");
			return false;
		}
		return true;
	}
	ModelRender			m_modelRender;				//���f�������_�\�B
	Vector3				m_position;					//���W�B
	Vector3				m_moveVector;				//�ړ������x�N�g���B
	Vector3				m_scale = Vector3::One;		//�X�P�[���B
	Quaternion			m_rotation;					//�N�H�[�^�j�I���B
	enum EnAnimationClip {							//�A�j���[�V�����B
		enAnimationClip_Idle,
		enAnimationClip_Run,
		enAnimationClip_Num,
	};
	AnimationClip		m_animationClips[enAnimationClip_Num];		//�A�j���[�V�����N���b�v�B
	EnEnemyState		m_enemyState = enEnemyState_Idle;			//�G�l�~�[�̃X�e�[�g(���)��\���ϐ��B
	std::unique_ptr<MovedPath>	m_movedPath;
	Player*				m_player = nullptr;			//�v���C���[�B
	SpotLight m_spotLight;
	VolumeSpotLight m_volumeSpotLight;
};

