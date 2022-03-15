#pragma once

class Game;

class MuscleRender : public IGameObject
{
public:
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	/// <summary>
	/// ���f���̍ő吔��ݒ肷��B
	/// </summary>
	/// <param name="numMuscle"></param>
	void SetMaxMuscle(int maxMuscle)
	{
		m_maxMuscle = maxMuscle;
	}
	/// <summary>
	/// �C���X�^���V���O�f�[�^�̍X�V�B
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="rot"></param>
	/// <param name="scale"></param>
	void UpdateInstancingData(
		int instanceNo,
		const Vector3& pos,
		const Quaternion& rot, const Vector3& scale
	) {
		m_modelRender.UpdateInstancingData(instanceNo, pos, rot, scale);
	}
	/// <summary>
	/// �C���X�^���X��j���B
	/// </summary>
	/// <param name="instanceNo">�C���X�^���X�ԍ�</param>
	void RemoveInstance(int instanceNo)
	{
		m_modelRender.RemoveInstance(instanceNo);
	}
private:
	ModelRender		m_modelRender;	// ���f�������_���[�B
	int				m_maxMuscle = 0;	// ���̐�

	AnimationClip	m_animationClip;
	Game*			m_game = nullptr;
};

