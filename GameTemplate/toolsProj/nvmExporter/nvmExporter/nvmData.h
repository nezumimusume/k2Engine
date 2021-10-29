#pragma once
#include <vector>

//1�|���S���̃f�[�^
struct SData
{
	Point3				position[3];		//3�̒��_�̍��W
	Point3				normal;				//�@��
	std::vector<int>	linkNoList;			//�����N�̃��X�g
};

class nvmData
{
public:
	static const int LINK_NO_LIST_SIZE = 3;
	nvmData();
	//�|���S�������擾
	const int GetFaceNum() const
	{
		return m_sDataPtrList.size();
	}
	const Point3& GetVertexPosition(int faceNo, int vertexNo) const
	{
		if (vertexNo > 2 || faceNo >= m_sDataPtrList.size()) {
			std::abort();
		}
		return m_sDataPtrList[faceNo]->position[vertexNo];
	}
	const Point3& GetNormal(int faceNo) const
	{
		if (faceNo >= m_sDataPtrList.size())
		{
			std::abort();
		}
		return m_sDataPtrList[faceNo]->normal;
	}
	const int GetLinkNo(int faceNo, int linkListNo) const
	{
		if (linkListNo > LINK_NO_LIST_SIZE - 1 || faceNo >= m_sDataPtrList.size())
		{
			std::abort();
		}
		return m_sDataPtrList[faceNo]->linkNoList[linkListNo];
	}
private:
	void Init();
	TriObject* GetTriObjectFromNode(INode* node);
	void CalcNormal(Point3& normal, Face& face, Mesh& mesh, Matrix3& matrix);
	bool IsWall(Point3& normal);
	void CreatePositionAndNormalData();
	//�����N�f�[�^�̍쐬
	void CreateLinkData();
private:
	std::vector<std::shared_ptr<SData>>				m_sDataPtrList;
};

