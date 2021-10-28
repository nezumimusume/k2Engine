#pragma once
#include <vector>

//1ポリゴンのデータ
struct SData
{
	Point3				position[3];		//3つの頂点の座標
	Point3				normal;				//法線
	std::vector<int>	linkNoList;			//リンクのリスト
};

class nvmData
{
public:
	static const int LINK_NO_LIST_SIZE = 3;
	nvmData();
	//ポリゴン数を取得
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
	//リンクデータの作成
	void CreateLinkData();
private:
	std::vector<std::shared_ptr<SData>>				m_sDataPtrList;
};

