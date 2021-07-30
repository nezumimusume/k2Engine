#pragma once
class MuscleRender;

class Muscle : public IGameObject
{
public:
	Muscle();
	~Muscle();
	bool Start();
	void Update();
	
	Vector3					m_position;					//���W�B
	Vector3					m_scale;					//�傫���B
	Quaternion				m_rotation;					//��]�B
	MuscleRender*			m_muscleRender = nullptr;	//�X�t�B�A�����_�[�B
};

