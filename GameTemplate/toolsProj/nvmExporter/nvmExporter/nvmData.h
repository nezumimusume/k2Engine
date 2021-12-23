#pragma once
#include <vector>
#include "bsp/BSP.h"

//1ポリゴンのデータ
struct SData
{
	Point3				position[3];		// 3つの頂点の座標
	Point3				normal;				// 法線
	std::vector<int>	linkNoList;			// リンクのリスト
	Vector3				centerPosition;		// 3つの頂点の中心座標
	int					faceNo;				// 番号
};

/// <summary>
/// nvmDataクラス
/// </summary>
class nvmData
{
private:
	using SDataPtr = std::shared_ptr<SData>;
public:
	/// <summary>
	/// リンクNoのリストのサイズ
	/// </summary>
	static const int LINK_NO_LIST_SIZE = 3;
	/// <summary>
	/// コンストラクタ
	/// </summary>
	nvmData();
	
	/// <summary>
	/// ポリゴン数を取得。
	/// </summary>
	/// <returns></returns>
	const int GetFaceNum() const
	{
		return m_sDataPtrList.size();
	}
	/// <summary>
	/// 頂点座標を取得
	/// </summary>
	/// <param name="faceNo">面の番号</param>
	/// <param name="vertexNo">頂点番号</param>
	/// <returns>頂点座標</returns>
	const Point3& GetVertexPosition(int faceNo, int vertexNo) const
	{
		if (vertexNo > 2 || faceNo >= m_sDataPtrList.size()) {
			std::abort();
		}
		return m_sDataPtrList[faceNo]->position[vertexNo];
	}
	/// <summary>
	/// 法線を取得
	/// </summary>
	/// <param name="faceNo">面の番号</param>
	/// <returns>法線</returns>
	const Point3& GetNormal(int faceNo) const
	{
		if (faceNo >= m_sDataPtrList.size())
		{
			std::abort();
		}
		return m_sDataPtrList[faceNo]->normal;
	}
	/// <summary>
	/// 隣接番号を取得
	/// </summary>
	/// <param name="faceNo">面の番号</param>
	/// <param name="linkListNo">隣接番号</param>
	/// <returns>隣接番号</returns>
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
	/// 初期化
	/// </summary>
	void Init();
	/// <summary>
	/// 三角形オブジェクトをノードから取得
	/// </summary>
	/// <param name="node">ノード</param>
	/// <returns></returns>
	TriObject* GetTriObjectFromNode(INode* node);
	/// <summary>
	/// 法線を計算
	/// </summary>
	/// <param name="normal">法線</param>
	/// <param name="face">面</param>
	/// <param name="mesh">メッシュ</param>
	/// <param name="matrix">行列</param>
	void CalcNormal(Point3& normal, Face& face, Mesh& mesh, Matrix3& matrix);
	/// <summary>
	/// 壁かどうか判定
	/// </summary>
	/// <param name="normal">法線</param>
	/// <returns>法線ならtrueを返す</returns>
	bool IsWall(Point3& normal);
	/// <summary>
	/// 座標と法線データを計算する
	/// </summary>
	void CreatePositionAndNormalData();
	/// <summary>
	/// リンクデータの作成
	/// </summary>
	void CreateLinkData();
	void CreateLinkData(std::vector<SDataPtr>& cellDataArray);
	void CreateLinkDataOneCell(SDataPtr cellData);
private:
	std::vector<SDataPtr>	m_sDataPtrList;			// セルデータの配列。
	BSP						m_bpsOnVertexPosition;	// セルの頂点座標のBSPツリー
};

