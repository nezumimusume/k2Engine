#include "k2EngineLowPreCompile.h"
#include "AI/PathFinding/NaviMesh.h"
#include "tkFile/TknFile.h"

namespace nsK2EngineLow {
namespace nsAI {
	void NaviMesh::Init(const char* tknFilePath)
	{
		TknFile tknFile;
		tknFile.Load(tknFilePath);
		// tknファイルからセル情報を構築する。
		int numCell = tknFile.GetNumCell();
		m_cellArray.resize(numCell);
		// セルの配列を構築する。
		for (int cellNo = 0; cellNo < numCell; cellNo++) {
			const auto& cellLow = tknFile.GetCell(cellNo);
			// 頂点データを設定する。
			m_cellArray[cellNo].SetVertexPosition(0, cellLow.vertexPosition[0]);
			m_cellArray[cellNo].SetVertexPosition(1, cellLow.vertexPosition[1]);
			m_cellArray[cellNo].SetVertexPosition(2, cellLow.vertexPosition[2]);
			// 法線を設定する。
			m_cellArray[cellNo].SetNormal(cellLow.normal);
			// 中心座標を計算する。
			auto centerPos = cellLow.vertexPosition[0]
				+ cellLow.vertexPosition[1]
				+ cellLow.vertexPosition[2];
			centerPos /= 3.0f;
			m_cellArray[cellNo].SetCenterPosition(centerPos);
			// 隣接セル情報を設定する。
			for (int linkNo = 0; linkNo < 3; linkNo++) {
				if (cellLow.linkCellNo[linkNo ] != -1) {
					m_cellArray[cellNo].SetLinkCell(linkNo, &m_cellArray[cellLow.linkCellNo[linkNo]]);
				}
				else {
					m_cellArray[cellNo].SetLinkCell(linkNo, nullptr);
				}
			}
			m_cellArray[cellNo].SetCellNo(cellNo);
		}
		// セルの中心座標を利用したBSPツリーを構築する。
		for ( auto& cell : m_cellArray) {
			// リーフを追加。
			m_cellCenterPosBSP.AddLeaf(
				cell.GetCenterPosition(),
				&cell
			);
		}
		m_cellCenterPosBSP.Build();
	}
	const Cell& NaviMesh::FindNearestCell(const Vector3& pos) const
	{
		const Cell* nearestCell = nullptr;

		float dist = FLT_MAX;
		m_cellCenterPosBSP.WalkTree(pos, [&](BSP::SLeaf* leaf) {
			Cell* cell = static_cast<Cell*>(leaf->extraData);
			auto distTmp = (cell->GetCenterPosition() - pos).Length();
			if (distTmp < dist) {
				//こちらの方が近い。
				dist = distTmp;
				nearestCell = cell;
			}
		});
		return *nearestCell;
	}
}
}


