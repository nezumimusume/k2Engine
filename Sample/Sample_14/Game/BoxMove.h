#pragma once
class BoxMove : public IGameObject
{
public:
	BoxMove();
	~BoxMove();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	ModelRender				m_modelRender;				//���f�������_���[�B
	Vector3					m_position;					//���W�B
	Vector3					m_scale;					//�傫���B
	Quaternion				m_rotation;					//��]�B
	PhysicsStaticObject		m_physicsStaticObject;		//�ÓI�����I�u�W�F�N�g�B

	Vector3					m_defPosition;				//�������W�B
	const float				m_move = 20.0f;				//�㉺�ړ��ʂ̏���B
	bool					m_moveflag = false;			//�㉺�ړ��̃t���O�B
};

