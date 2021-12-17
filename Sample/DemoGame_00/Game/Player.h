#pragma once

#include "graphics/VolumeSpotLight.h"

class Player : public IGameObject
{
public:
	enum EnPlayerState
	{
		enPlayerState_Idle,					//�ҋ@���B
		enPlayerState_Run,					//����B
		enPlayerState_Jump,					//�W�����v���B
		enPlayerState_GameOver,				//�Q�[���I�[�o�[�B
		enPlayerState_GameClear,			//�Q�[���N���A�[�B
		enPlayerState_GameClear_Idle,		//�Q�[���N���A(�ҋ@��)�B
		enPlayerState_Num
	};
public:
	////////////////////////////////////
	// �����o�֐�
	////////////////////////////////////
	Player();
	~Player();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

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
	/// �������W��ݒ�B
	/// </summary>
	/// <param name="startPosition">�������W�B</param>
	void SetStartPosition(const Vector3& startPosition)
	{
		m_startPosition = startPosition;
	}
	/// <summary>
	/// �v���C���[�̃X�e�[�g(���)���擾�B
	/// </summary>
	/// <returns>�v���C���[�̏�ԁB</returns>
	const EnPlayerState GetPlayerState() const
	{
		return m_playerState;
	}
	const Vector3& GetForward() const
	{
		return m_forward;
	}
	void NotifyGameOver();
	void NotifyGameClear();
	void ReStart();
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
	ModelRender			m_modelRender;				//���f�������_�\�B
	Vector3				m_position;					//���W�B
	Vector3				m_startPosition;			//�������W�B
	CharacterController m_charaCon;					//�L�����N�^�[�R���g���[���[�B
	Vector3				m_moveSpeed;				//�ړ����x�B
	Quaternion			m_rotation;					//�N�H�[�^�j�I���B
	enum EnAnimationClip {							//�A�j���[�V�����B
		enAnimationClip_Idle,
		enAnimationClip_Run,
		enAnimationClip_Jump,
		enAnimationClip_GameClear,
		enAnimationClip_GameOver,
		enAnimationClip_Num,
	};
	AnimationClip		m_animationClips[enAnimationClip_Num];		//�A�j���[�V�����N���b�v�B
	EnPlayerState		m_playerState = enPlayerState_Idle;				//�v���C���[�̃X�e�[�g(���)��\���ϐ��B
	Vector3				m_forward;
	std::vector<SPointLight* > m_pointLightList;
	SpotLight m_spotLight;				// �X�|�b�g���C�g�B
	
	VolumeSpotLight m_volumeSpotLight;	// �{�����[���X�|�b�g���C�g�B
};

