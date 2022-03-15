#pragma once
class MuscleRender;
class Game;

class Muscle : public IGameObject
{
public:
	Muscle();
	~Muscle();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	
	Vector3					m_position;					// ���W�B
	Vector3					m_scale;					// �傫���B
	Quaternion				m_rotation;					// ��]�B
	MuscleRender*			m_muscleRender = nullptr;	// �X�t�B�A�����_�[�B
	ModelRender				m_modelRender;
	Game*					m_game = nullptr;
	AnimationClip			m_animationClip;
	int						m_instanceNo = 0;			// �C���X�^���X�ԍ�
};

