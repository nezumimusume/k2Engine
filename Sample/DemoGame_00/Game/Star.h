#pragma once

//�N���X�錾�B
class Player;
class StarRender;

class Star : public IGameObject
{
public:
	~Star();
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
	/// �C���X�^���X�ԍ���ݒ�B
	/// </summary>
	/// <param name="instanceNo"></param>
	void SetInstanceNo(int instanceNo)
	{
		m_instanceNo = instanceNo;
	}
private:
	Vector3				m_position;						//���p�B
	Vector3				m_scale = Vector3::One * 0.3f;	//�傫���B
	Player*				m_player = nullptr;
	bool				m_isGet = false;
	float				m_jumpSpeed = 0.0f;
	float				m_jumStartPosY = 0.0f;
	Quaternion			m_rotation;
	StarRender*			m_starRender = nullptr;
	int					m_instanceNo = 0;				// �C���X�^���X�ԍ��B
};

