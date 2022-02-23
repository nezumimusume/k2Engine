#pragma once
class StarRender : public IGameObject
{
public:
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	
	/// <summary>
	/// ���̍ő吔��ݒ肷��B
	/// </summary>
	/// <param name="numStar"></param>
	void SetMaxStar(int maxStar)
	{
		m_maxStar = maxStar;
	}
	/// <summary>
	/// �C���X�^���X�������B
	/// </summary>
	/// <param name="instanceNo"></param>
	void RemoveInstance(int instanceNo)
	{
		m_modelRender.RemoveInstance(instanceNo);
	}
	/// <summary>
	/// �C���X�^���V���O�f�[�^�̍X�V�B
	/// </summary>
	/// <param name="instanceNo">�C���X�^���X�̔ԍ�</param>
	/// <param name="pos"></param>
	/// <param name="rot"></param>
	/// <param name="scale"></param>
	void UpdateInstancingData(
		int instanceNo,
		const Vector3& pos,
		const Quaternion& rot, 
		const Vector3& scale
	) {
		m_modelRender.UpdateInstancingData(instanceNo, pos, rot, scale);
	}
private:
	ModelRender	m_modelRender;	// ���f�������_���[�B
	int			m_maxStar = 0;	// ���̐�
};

