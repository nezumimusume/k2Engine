

class Player : public IGameObject
{
public:
	////////////////////////////////////
	// �����o�֐�
	////////////////////////////////////
	Player();
	~Player();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
	const Vector3& GetPosition() const
	{
		return m_position;
	}
private:
	void Move();
	void Rotation();
	////////////////////////////////////
	// �����o�ϐ��Ƃ�
	////////////////////////////////////
	ModelRender					m_modelRender;
	Vector3						m_position;						//���W�B
	CharacterController			m_charaCon;						//�L�����N�^�[�R���g���[���[�B
	Vector3						m_moveSpeed;					//�ړ����x�B
	Quaternion					m_rotation;						//��]�B
	Vector3						m_forward = Vector3::AxisZ;
	SpotLight					m_spotLight;					//�X�|�b�g���C�g�B
	VolumeSpotLight				m_volumeSpotLight;				//�{�����[�����C�g�B
};

