#include "k2EngineLowPreCompile.h"
#include "AI/PathFinding/PathFinding.h"
#include "AI/PathFinding/NaviMesh.h"

namespace nsK2EngineLow {
	namespace nsAI {
		void PathFinding::Execute(
			Path& path,
			const NaviMesh& naviMesh,
			const Vector3& startPos,
			const Vector3& endPos
		)
		{
			const auto& startCell = naviMesh.FindNearestCell(startPos);
			const auto& endCell = naviMesh.FindNearestCell(endPos);
			// 
			m_cellWork.resize(naviMesh.GetNumCell());
			for (int cellNo = 0; cellNo < m_cellWork.size(); cellNo++) {
				m_cellWork[cellNo].m_cell = &naviMesh.GetCell(cellNo);
			}
			
			
			using namespace std;
			const int OPEN_CLOSE_LIST_MAX = 1024;
			vector< Cell* > openList, closeList;
			openList.reserve(OPEN_CLOSE_LIST_MAX);
		
		}
	}
}


