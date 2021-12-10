#pragma once

class Background : public IGameObject
{
public:
	//////////////////////////////////////
	// �����o�֐��B
	//////////////////////////////////////
	Background();
	~Background();
	bool Start();
	
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
	/// �傫����ݒ�B
	/// </summary>
	/// <param name="scale">�傫���B</param>
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}
	/// <summary>
	/// ��]��ݒ�B
	/// </summary>
	/// <param name="rotation">��]�B</param>
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
private:
	ModelRender			m_modelRender;							//���f�������_���[�B
	PhysicsStaticObject m_phyStaticObject;						//�ÓI�����I�u�W�F�N�g�B
	Vector3				m_position;								//���W�B
	Quaternion			m_rotation = Quaternion::Identity;		//��]�B
	Vector3				m_scale = Vector3::One;					//�傫���B
};