#include "k2EngineLowPreCompile.h"
#include "AI/PathFinding/NaviMesh.h"
#include "tkFile/TknFile.h"

namespace nsK2EngineLow {
namespace nsAI {
	void NaviMesh::Init(const char* tknFilePath)
	{
		TknFile tknFile;
		tknFile.Load(tknFilePath);
		// tkn�t�@�C������Z�������\�z����B
		int numCell = tknFile.GetNumCell();
		m_cellArray.resize(numCell);
		// �Z���̔z����\�z����B
		for (int cellNo = 0; cellNo < numCell; cellNo++) {
			const auto& cellLow = tknFile.GetCell(cellNo);
			// ���_�f�[�^��ݒ肷��B
			m_cellArray[cellNo].SetVertexPosition(0, cellLow.vertexPosition[0]);
			m_cellArray[cellNo].SetVertexPosition(1, cellLow.vertexPosition[1]);
			m_cellArray[cellNo].SetVertexPosition(2, cellLow.vertexPosition[2]);
			// �@����ݒ肷��B
			m_cellArray[cellNo].SetNormal(cellLow.normal);
			// ���S���W���v�Z����B
			auto centerPos = cellLow.vertexPosition[0]
				+ cellLow.vertexPosition[1]
				+ cellLow.vertexPosition[2];
			centerPos /= 3.0f;
			m_cellArray[cellNo].SetCenterPosition(centerPos);
			// �אڃZ������ݒ肷��B
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
		// �Z���̒��S���W�𗘗p����BSP�c���[���\�z����B
		for ( auto& cell : m_cellArray) {
			// ���[�t��ǉ��B
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
				//������̕����߂��B
				dist = distTmp;
				nearestCell = cell;
			}
		});
		return *nearestCell;
	}
}
}


