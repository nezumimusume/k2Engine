#pragma once


/// <summary>
/// ���o�[�B
/// </summary>
class Lever : public IGameObject
{
public:
	/// <summary>
	/// ���o�[�X�e�[�g�B
	/// </summary>
	enum EnLeverState {
		enLeverState_Idle,				//�ŏ��̑ҋ@��ԁB
		enLeverState_Push,				//�����B
		enLeverState_Push_Idle,			//�����I������B
		enLeverState_Pull,				//�����B
		enLeverState_Pull_Idle			//�����I������B
	};
public:
	Lever();
	~Lever();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	/// <summary>
	/// ���W��ݒ肷��B
	/// </summary>
	/// <param name="position">���W�B</param>
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	/// <summary>
	/// �傫����ݒ肷��B
	/// </summary>
	/// <param name="scale">�傫���B</param>
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}
	/// <summary>
	/// ��]��ݒ肷��B
	/// </summary>
	/// <param name="rotation">��]�B</param>
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
	/// <summary>
	/// ���o�[�̔ԍ���ݒ肷��B
	/// �ݒ肵���ԍ��Ɠ����h�A�𓮂����܂��B
	/// </summary>
	/// <param name="leverNumber">�ԍ��B</param>
	void SetLeverNumber(const int leverNumber)
	{
		m_leverNumber = leverNumber;
	}
	/// <summary>
	/// ���o�[�̔ԍ����擾����B
	/// </summary>
	/// <returns>�ԍ��B</returns>
	const int GetLeverNumber() const
	{
		return m_leverNumber;
	}
private:
	/// <summary>
	/// �����X�e�[�g�Ɉڍs���鏈���B
	/// </summary>
	void ProcessTransitionPushState();
	/// <summary>
	/// �����I������X�e�[�g�Ɉڍs���鏈���B
	/// </summary>
	void ProcessTransitionPushIdleState();
	/// <summary>
	/// �����X�e�[�g�Ɉڍs���鏈���B
	/// </summary>
	void ProcessTransitionPullState();
	/// <summary>
	/// �����I������X�e�[�g�ɍs�������鏈���B
	/// </summary>
	void ProcessTransitionPullIdleState();
	/// <summary>
	/// �A�j���[�V�������Đ����鏈���B
	/// </summary>
	void PlayAnimation();
	/// <summary>
	/// �X�e�[�g�Ǘ��B
	/// </summary>
	void ManageState();
	/// <summary>
	/// �ҋ@��Ԃ̃X�e�[�g�J�ڏ����B
	/// </summary>
	void ProcessIdleStateTransition();
	/// <summary>
	/// �����X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessPushStateTransition();
	/// <summary>
	/// �����I������X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessPushIdleStateTransition();
	/// <summary>
	/// �����X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessPullStateTransition();
	/// <summary>
	/// �����I������X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessPullIdleStateTransition();
	
	enum EnAnimationClip {					//�A�j���[�V�����B
		enAnimationClip_Idle,				//�ҋ@�B
		enAnimationClip_Push,				//�����B
		enAnimationClip_Pull,				//�����B
		enAnimationClip_Num,				//�A�j���[�V�����̐��B
	};
	AnimationClip			m_animationClips[enAnimationClip_Num];		//�A�j���[�V�����N���b�v�B
	ModelRender				m_modelRender;								//���f�������_�[�B
	Vector3					m_position;									//���W�B
	Vector3					m_scale = Vector3::One;						//�傫���B
	Quaternion				m_rotation;									//��]�B
	CollisionObject*		m_collisionObject = nullptr;				//�R���W�����I�u�W�F�N�g�B
	EnLeverState			m_leverState = enLeverState_Idle;			//���o�[�X�e�[�g�B
	int						m_leverNumber = 0;							//���o�[�̔ԍ��B
};

