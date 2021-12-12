#pragma once

/// <summary>
/// �p�X�̃|�C���g�B
/// </summary>
struct Point {
	Vector3		s_vector = Vector3::Zero;		//���W�B
	int			s_number = 0;					//�ԍ��B
};

class Path
{
public:
	/// <summary>
	/// �R���X�g���N�^�B
	/// </summary>
	Path();
	/// <summary>
	/// �f�X�g���N�^�B
	/// </summary>
	~Path();
	/// <summary>
	/// �p�X�f�[�^��ǂݍ��ށB
	/// </summary>
	/// <param name="filePath">�t�@�C���p�X�B</param>
	void Load(const char* filePath);
	/// <summary>
	/// �p�X�̃|�C���g�̐����擾�B
	/// </summary>
	/// <returns>�p�X�̃|�C���g�̐��B</returns>
	const int GetNumberPoint() const
	{
		return static_cast<const int>(m_pointMap.size());
	}
	/// <summary>
	/// �ŏ��̃|�C���g���擾�B
	/// </summary>
	/// <returns> �ŏ��̃|�C���g�B</returns>
 	const Point* GetFistPoint()
	{
		return &m_pointMap[0];
	}
	/// <summary>
	/// ���݂̍��W�����ԋ߂��|�C���g���擾�B
	/// </summary>
	/// <param name="position">���W�B</param>
	/// <returns>�|�C���g�B</returns>
	const Point* GetNearPoint(const Vector3& position);
	/// <summary>
	/// ���̃|�C���g���擾�B
	/// </summary>
	/// <param name="number">���݂̃|�C���g�̔ԍ��B</param>
	/// <returns>�|�C���g�B</returns>
	const Point* GetNextPoint(const int number);
private:
	std::map<const int ,Point>		m_pointMap;			//�|�C���g�̃}�b�v�B
};

