#pragma once


/// <summary>
/// �t�@�C���[�{�[���B
/// </summary>
class FireBall : public IGameObject
{
public:
	//�t�@�C���[�{�[�����������p�ҁB
	enum EnMagician {
		enMagician_None,
		enMagician_Player,			//�v���C���[�B
		enMagician_Enemy			//�G�l�~�[�B
	};
public:
	FireBall();
	~FireBall();
	bool Start();
	void Update();
	/// <summary>
	/// ���W��ݒ肷��B
	/// </summary>
	/// <param name="position">���W�B</param>
	void SetPosition(const Vector3& position)
	{
		m_position = position;
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
	/// �傫����ݒ肷��B
	/// </summary>
	/// <param name="scale">�傫���B</param>
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}
	/// <summary>
	/// �p�҂�ݒ肷��B
	/// </summary>
	/// <param name="enMagician">�p�ҁB</param>
	void SetEnMagician(const EnMagician enMagician)
	{
		m_enMagician = enMagician;
	}
private:
	Vector3					m_position;							//���W�B
	Quaternion				m_rotation;							//��]�B
	Vector3					m_scale = Vector3::One;				//�傫���B
	Vector3					m_moveSpeed;						//�ړ����x�B
	EffectEmitter*			m_effectEmitter = nullptr;			//�G�t�F�N�g�B
	float					m_timer = 0.0f;						//�^�C�}�[�B
	CollisionObject*		m_collisionObject;					//�R���W�����I�u�W�F�N�g�B
	EnMagician				m_enMagician = enMagician_None;		//�p�ҁB
};

