#include "k2EngineLowPreCompile.h"
#include "AI/PathFinding/PathFinding.h"
#include "AI/PathFinding/NaviMesh.h"
#include "AI/PathFinding/Path.h"

namespace nsK2EngineLow {
namespace nsAI {
	
	void PathFinding::Smoothing(std::list<CellWork*>& cellList) 
	{
		// パスの可視判定を行って、不要なセルを除外していく。
		if (cellList.size() < 3) {
			// セルの数が3以下ならスムージングする必要なし。
			return;
		}

		// セルが2より大きければ、パスの可視判定を行って、不要なセルを除外していく。
		// レイの始点となるセル。
		auto rayStartCellIt = cellList.begin();
		// レイの終点の手前のセル。
		auto prevRayEndCellIt = rayStartCellIt;
		prevRayEndCellIt++;
		// レイの終点のセル。
		auto rayEndCellIt = prevRayEndCellIt;
		rayEndCellIt++;

		auto endCellIt = cellList.end();
		endCellIt--;
		auto prevEndCellIt = endCellIt;
		prevEndCellIt--;

		while (true) {
			// レイの終端のセルまでレイテストを行う。
			Vector3 rayStartPos = (*rayStartCellIt)->cell->GetCenterPosition();
			Vector3 rayEndPos = (*rayEndCellIt)->cell->GetCenterPosition();

			// todo ここに可視判定を実装する。

			if (false) {
				// 可視
				// 始点から終点が見えるということは、終点の手前のセルは削除できる。
				cellList.erase(prevRayEndCellIt);
				// 次。
				prevRayEndCellIt = rayEndCellIt;
				rayEndCellIt++;
			}
			else {
				// 見えないということは、終端セルまでのスムージングをこれ以上行うことはできない。
				if (rayEndCellIt == endCellIt
					|| rayEndCellIt == prevEndCellIt
				) {
					// 終わり。
					break;
				}
				// 現在の終端セルを
				rayStartCellIt = rayEndCellIt;
				prevRayEndCellIt = rayStartCellIt;
				prevRayEndCellIt++;
				rayEndCellIt = prevRayEndCellIt;
				rayEndCellIt++;
			}
		}
	}
	/// <summary>
	/// 次のセルに移動するコストを計算
	/// </summary>
	/// <returns></returns>
	void PathFinding::CalcCost(
		float& totalCost, 
		float& costFromStartCell, 
		const CellWork* nextCell, 
		const CellWork* prevCell, 
		const Cell* endCell
	)
	{
		if (prevCell == nullptr) {
			// 直前のセルがない。
			costFromStartCell = 0.0f;
		}
		else {
			// 直前のセルがある。
			Vector3 t = nextCell->cell->GetCenterPosition() - prevCell->cell->GetCenterPosition();
			costFromStartCell = t.Length() + prevCell->costFromStartCell;
		}
		float distToEndPos = (nextCell->cell->GetCenterPosition() - endCell->GetCenterPosition()).Length();
		totalCost = costFromStartCell + distToEndPos;
	}
	
	void PathFinding::Execute(
		Path& path,
		const NaviMesh& naviMesh,
		const Vector3& startPos,
		const Vector3& endPos
	)
	{
		path.Clear();
		const Cell& startCell = naviMesh.FindNearestCell(startPos);
		const Cell& endCell = naviMesh.FindNearestCell(endPos);
		// 
		m_cellWork.resize(naviMesh.GetNumCell());
		// 初期化する。
		for (int cellNo = 0; cellNo < m_cellWork.size(); cellNo++) {
			m_cellWork[cellNo].Init(&naviMesh.GetCell(cellNo));
		}
			
			
		using namespace std;
		const int OPEN_CLOSE_LIST_MAX = 1024;
		vector< CellWork* > openList;
		openList.reserve(OPEN_CLOSE_LIST_MAX);
			
		CellWork* crtCell = &m_cellWork.at( startCell.GetCellNo() );
		// 開始セルの移動コストを計算する。
		CalcCost(crtCell->cost, crtCell->costFromStartCell, crtCell, nullptr, &endCell);
		crtCell->isOpend = true;

		// 開始セルをオープンリストに積む。
		openList.emplace_back(crtCell);

		bool isSuccess = false;
		while (!openList.empty()) {
			// オープンリストから最もコストの安いセルを取得。
			crtCell = nullptr;
			float lowestCost = FLT_MAX;
			vector<CellWork*>::iterator lowestCellIt;
			for( auto it = openList.begin(); it != openList.end(); it++ ){
				if ((*it)->isClosed) {
					int hoge = 0;
				}
				if (lowestCost > (*it)->cost) {
					// このセルの方が安い。
					lowestCost = (*it)->cost;
					crtCell = (*it);
					lowestCellIt = it;
				}
			}
			// 取り出したセルをオープンリストから削除。
			openList.erase(lowestCellIt);
			if (crtCell->cell == &endCell) {
				// 経路探索成功
				isSuccess = true;
				break;
			}
				
			// 隣接セルをオープンリストに積んでいく。
			for (int linkCellNo = 0; linkCellNo < 3; linkCellNo++) {
				Cell* linkCell = crtCell->cell->GetLinkCell(linkCellNo);
				if( linkCell != nullptr){
					// 隣接セルがあったので、コストを計算してオープンリストに積む。
					CellWork* linkCellWork = &m_cellWork.at(linkCell->GetCellNo());

					// 現在のセルから隣接セルに移動するコストを計算
					float newCost;
					float newCostFromStartCell;
					CalcCost(newCost, newCostFromStartCell, linkCellWork, crtCell, &endCell);
					
					if (linkCellWork->isClosed == false	) {
						if (linkCellWork->cost > newCost) {
							// こちらのコストの方が安いので改善する。
							linkCellWork->costFromStartCell = newCostFromStartCell;
							linkCellWork->cost = newCost;
							// 親を設定。
							linkCellWork->parentCell = crtCell;
						}
						if (linkCellWork->isOpend == false) {
							// まだ開いていないなら
							linkCellWork->isOpend = true;
							openList.emplace_back(linkCellWork);
						}
							
					}
						
				}
			}
			// このセルの調査は終了なので、クローズのしるしをつける。
			crtCell->isClosed = true;
		}
			
		if (isSuccess) {
			list<CellWork*> cellList;
			crtCell = &m_cellWork.at(endCell.GetCellNo());
			while (true) {
				if (crtCell->cell == &startCell) {
					// 開始セルに到達した。
					// 最後に開始セルを積む。
					cellList.emplace_back(crtCell);
					break;
				}
				cellList.emplace_back(crtCell);
				crtCell = crtCell->parentCell;
			}
			// 反転させる。
			std::reverse(cellList.begin(), cellList.end());
			Smoothing(cellList);
			
			// ポイントをパスに積んでいく
			for (auto it = cellList.begin(); it != cellList.end(); it++) {
				path.AddPoint((*it)->cell->GetCenterPosition());
			}
			// パスを構築する。
			path.Build();
				
		}
	}
}
}


