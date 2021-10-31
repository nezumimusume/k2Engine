#include "k2EngineLowPreCompile.h"
#include "AI/PathFinding/PathFinding.h"
#include "AI/PathFinding/NaviMesh.h"
#include "AI/PathFinding/Path.h"

namespace nsK2EngineLow {
	namespace nsAI {
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
			// ノードのコストを計算する。
			// 開始セルからの移動コスト。単純に距離。
			crtCell->costFromStartCell = 0.0f;
			// セル自身のコストはヒューリスティックコスト＋開始セルからの移動コスト
			// このエンジンのA*のヒューリスティックコストは目標セルまでの距離とする。
			float distToEndPos = (startCell.GetCenterPosition() - endCell.GetCenterPosition()).Length();
			crtCell->cost = crtCell->costFromStartCell + distToEndPos;
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
						Vector3 t = linkCellWork->cell->GetCenterPosition() - crtCell->cell->GetCenterPosition();
						float newCostFromStartCell = t.Length() + crtCell->costFromStartCell;
						float distToEndPos = (linkCellWork->cell->GetCenterPosition() - endCell.GetCenterPosition()).Length();
						float newCost = newCostFromStartCell + distToEndPos;
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
				// startからendへの経路が見つかった。
				// パスを構築していく。
				vector<Vector3> pointArray;
				// ゴールセルからスタートセルに到達するまで、親をたどっていく。
				crtCell = &m_cellWork.at(endCell.GetCellNo());
				while (true) {
					if (crtCell->cell == &startCell) {
						// 開始セルに到達した。
						break;
					}
					pointArray.emplace_back(crtCell->cell->GetCenterPosition());
					crtCell = crtCell->parentCell;
				}
				// 最後に開始セルを追加。
				pointArray.push_back(startCell.GetCenterPosition());
				// ポイントをパスに積んでいく
				for (auto it = pointArray.rbegin(); it != pointArray.rend(); it++) {
					path.AddPoint(*it);
				}
				// パスを構築する。
				path.Build();
				
			}
		}
	}
}


