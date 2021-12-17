

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
	void Move();
	void Turn();
	////////////////////////////////////
	// �����o�ϐ��Ƃ�
	////////////////////////////////////
	ModelRender m_modelRender;
	Vector3 m_position;						// ���W�B
	Vector3	m_scale = Vector3::One;			// �傫���B
	CharacterController m_charaCon;			// �L�����N�^�[�R���g���[���[�B
	Vector3 m_moveSpeed;					// �ړ����x�B
	Quaternion m_rotation;					// �N�H�[�^�j�I���B
	PointLight m_pointLight;				// �|�C���g���C�g�B
};

