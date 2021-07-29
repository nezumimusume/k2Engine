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
	/// �C���X�^���V���O�f�[�^�̍X�V�B
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="rot"></param>
	/// <param name="scale"></param>
	void UpdateInstancingData(
		const Vector3& pos,
		const Quaternion& rot, const Vector3& scale
	) {
		m_modelRender.UpdateInstancingData(pos, rot, scale);
	}
private:
	ModelRender	m_modelRender;	// ���f�������_���[�B
	int			m_maxStar = 0;	// ���̐�
};

