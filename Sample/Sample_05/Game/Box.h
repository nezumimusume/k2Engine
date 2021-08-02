#pragma once
class Box : public IGameObject
{
public:
	Box();
	~Box();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	ModelRender				m_modelRender;				//���f�������_���[�B
	Vector3					m_position;					//���W�B
	Vector3					m_scale;					//�傫���B
	Quaternion				m_rotation;					//��]�B
	PhysicsStaticObject		m_physicsStaticObject;		//�ÓI�����I�u�W�F�N�g�B
	PointLight*				m_pointLight;				//�|�C���g���C�g�B
	int						m_lightNumber = 0;
};

