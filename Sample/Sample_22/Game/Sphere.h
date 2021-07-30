#pragma once
class SphereRender;

class Sphere : public IGameObject
{
public:
	Sphere();
	~Sphere();
	bool Start();
	void Update();
	
	Vector3					m_position;					//���W�B
	Vector3					m_scale;					//�傫���B
	Quaternion				m_rotation;					//��]�B
	SphereRender*			m_sphereRender = nullptr;	//�X�t�B�A�����_�[�B
};

