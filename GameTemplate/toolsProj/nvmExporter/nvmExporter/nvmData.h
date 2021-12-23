#pragma once
#include <vector>
#include "bsp/BSP.h"

//1�|���S���̃f�[�^
struct SData
{
	Point3				position[3];		// 3�̒��_�̍��W
	Point3				normal;				// �@��
	std::vector<int>	linkNoList;			// �����N�̃��X�g
	Vector3				centerPosition;		// 3�̒��_�̒��S���W
	int					faceNo;				// �ԍ�
};

/// <summary>
/// nvmData�N���X
/// </summary>
class nvmData
{
private:
	using SDataPtr = std::shared_ptr<SData>;
public:
	/// <summary>
	/// �����NNo�̃��X�g�̃T�C�Y
	/// </summary>
	static const int LINK_NO_LIST_SIZE = 3;
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	nvmData();
	
	/// <summary>
	/// �|���S�������擾�B
	/// </summary>
	/// <returns></returns>
	const int GetFaceNum() const
	{
		return m_sDataPtrList.size();
	}
	/// <summary>
	/// ���_���W���擾
	/// </summary>
	/// <param name="faceNo">�ʂ̔ԍ�</param>
	/// <param name="vertexNo">���_�ԍ�</param>
	/// <returns>���_���W</returns>
	const Point3& GetVertexPosition(int faceNo, int vertexNo) const
	{
		if (vertexNo > 2 || faceNo >= m_sDataPtrList.size()) {
			std::abort();
		}
		return m_sDataPtrList[faceNo]->position[vertexNo];
	}
	/// <summary>
	/// �@�����擾
	/// </summary>
	/// <param name="faceNo">�ʂ̔ԍ�</param>
	/// <returns>�@��</returns>
	const Point3& GetNormal(int faceNo) const
	{
		if (faceNo >= m_sDataPtrList.size())
		{
			std::abort();
		}
		return m_sDataPtrList[faceNo]->normal;
	}
	/// <summary>
	/// �אڔԍ����擾
	/// </summary>
	/// <param name="faceNo">�ʂ̔ԍ�</param>
	/// <param name="linkListNo">�אڔԍ�</param>
	/// <returns>�אڔԍ�</returns>
	const int GetLinkNo(int faceNo, int linkListNo) const
	{
		if (linkListNo > LINK_NO_LIST_SIZE - 1 || faceNo >= m_sDataPtrList.size())
		{
			std::abort();
		}
		return m_sDataPtrList[faceNo]->linkNoList[linkListNo];
	}
private:
	/// <summary>
	/// ������
	/// </summary>
	void Init();
	/// <summary>
	/// �O�p�`�I�u�W�F�N�g���m�[�h����擾
	/// </summary>
	/// <param name="node">�m�[�h</param>
	/// <returns></returns>
	TriObject* GetTriObjectFromNode(INode* node);
	/// <summary>
	/// �@�����v�Z
	/// </summary>
	/// <param name="normal">�@��</param>
	/// <param name="face">��</param>
	/// <param name="mesh">���b�V��</param>
	/// <param name="matrix">�s��</param>
	void CalcNormal(Point3& normal, Face& face, Mesh& mesh, Matrix3& matrix);
	/// <summary>
	/// �ǂ��ǂ�������
	/// </summary>
	/// <param name="normal">�@��</param>
	/// <returns>�@���Ȃ�true��Ԃ�</returns>
	bool IsWall(Point3& normal);
	/// <summary>
	/// ���W�Ɩ@���f�[�^���v�Z����
	/// </summary>
	void CreatePositionAndNormalData();
	/// <summary>
	/// �����N�f�[�^�̍쐬
	/// </summary>
	void CreateLinkData();
	void CreateLinkData(std::vector<SDataPtr>& cellDataArray);
	void CreateLinkDataOneCell(SDataPtr cellData);
private:
	std::vector<SDataPtr>	m_sDataPtrList;			// �Z���f�[�^�̔z��B
	BSP						m_bpsOnVertexPosition;	// �Z���̒��_���W��BSP�c���[
};

