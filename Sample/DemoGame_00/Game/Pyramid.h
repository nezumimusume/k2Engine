#pragma once
//�N���X�錾�B
class Player;
class PyramidRender;

class Pyramid : public IGameObject
{
public:
	~Pyramid();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/// <summary>
	/// ���W��ݒ�B
	/// </summary>
	/// <param name="position">���W�B</param>
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	/// <summary>
	/// ���W���擾�B
	/// </summary>
	/// <returns>���W�B</returns>
	const Vector3& GetPosition() const
	{
		return m_position;
	}
	/// <summary>
	/// �g�嗦��ݒ�B
	/// </summary>
	/// <param name="scale"></param>
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}
	/// <summary>
	/// �C���X�^���X�ԍ���ݒ�B
	/// </summary>
	/// <param name="instanceNo">�C���X�^���X�ԍ��B</param>
	void SetInstanceNo(int instanceNo)
	{
		m_instanceNo = instanceNo;
	}
private:
	Vector3				m_position;						// ���W�B
	Vector3				m_scale = Vector3::One * 0.3f;	// �傫���B
	Player*				m_player = nullptr;
	bool				m_isGet = false;
	float				m_jumpSpeed = 0.0f;
	float				m_jumPyramidtPosY = 0.0f;
	Quaternion			m_rotation;
	PyramidRender*		m_pyramidRender = nullptr;
	int					m_instanceNo = 0;				// �C���X�^���X�ԍ��B
};

