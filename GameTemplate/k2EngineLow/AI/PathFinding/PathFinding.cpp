#include "k2EngineLowPreCompile.h"
#include "AI/PathFinding/PathFinding.h"
#include "AI/PathFinding/NaviMesh.h"
#include "AI/PathFinding/Path.h"

namespace nsK2EngineLow {
namespace nsAI {
	void PathFinding::CellWork::Init(const Cell* cell)
	{
		this->cell = cell;
		parentCell = nullptr;
		costFromStartCell = 0.0f;
		cost = FLT_MAX;
		isOpend = false;
		isClosed = false;
		isSmooth = false;
		pathPoint = cell->GetCenterPosition();
	}
	bool PathFinding::IsIntercetRayToCell(Vector3 rayStartPos, Vector3 rayEndPos, CellWork* currentCellWork) const
	{
		// レイを計算する。
		Vector3 ray = rayEndPos - rayStartPos;

		bool isVisible = false;

		// 可視ということは、rayStartPosからrayEndPosに向かって伸びる線分が、セルのどれか１つのエッジと交差しているということ。
		const int vertNo[3][2] = {
			{0, 1},
			{1, 2},
			{2, 0},
		};
		const Cell* currentCell = currentCellWork->cell;
		for (int edgeNo = 0; edgeNo < 3; edgeNo++) {
			const Vector3& p0 = currentCell->GetVertexPosition(vertNo[edgeNo][0]);
			const Vector3& p1 = currentCell->GetVertexPosition(vertNo[edgeNo][1]);
			// まずは無限線分として交差しているか判定。
			Vector3 p0ToStartPos = rayStartPos - p0;
			p0ToStartPos.y = 0.0f;
			Vector3 p0ToEndPos = rayEndPos - p0;
			p0ToEndPos.y = 0.0f;

			// p0ToStartPosとp0ToEndPosを正規化する。
			Vector3 p0ToStartPosNorm = p0ToStartPos;
			p0ToStartPosNorm.Normalize();
			Vector3 p0ToEndPosNorm = p0ToEndPos;
			p0ToEndPosNorm.Normalize();

			if (p0ToStartPosNorm.Dot(p0ToEndPosNorm) <= 0.0f) {
				// 交差している。
				// 続いて交点を求める。
				// まずは、XZ平面でレイに垂直な線分を求める。
				Vector3 rayNorm = ray;
				rayNorm.Normalize();
				Vector3 rayTangent;
				rayTangent.Cross(rayNorm, g_vec3AxisY);
				rayTangent.Normalize();
				float t0 = fabsf(Dot(rayTangent, p0ToStartPos));
				float t1 = fabsf(Dot(rayTangent, p0ToEndPos));
				// 始点から交点までの比率を求める。
				float rate = t0 / (t0 + t1);
				Vector3 hitPos = Math::Lerp(rate, rayStartPos, rayEndPos);
				// 続いて交点が線分上にいるか調べる。
				Vector3 rsToHitPos = hitPos - rayStartPos;
				Vector3 reToHitPos = hitPos - rayEndPos;
				rsToHitPos.Normalize();
				reToHitPos.Normalize();
				if (rsToHitPos.Dot(reToHitPos) <= 0.0f) {
					// 交差している場合はこのベクトルが逆向きになるはず。
					// 交差している。
					isVisible = true;
					break;
				}
			}
		}

		return isVisible;
	}
	void PathFinding::Smoothing(
			std::list<CellWork*>& cellList,
			PhysicsWorld* physicsWorld,
			float agentRadius,
			float agentHeight
	) {
		// パスの可視判定を行って、不要なセルを除外していく。
		if (cellList.size() < 3) {
			// セルの数が3以下ならスムージングする必要なし。
			return;
		}
		int skipCellCount = static_cast<int>(cellList.size()) - 1;
		while (skipCellCount > 2) {
			// セルの数が３以上なら、パスの可視判定を行って、不要なセルを除外していく。
			// レイの始点となるセル。
			auto rayStartCellIt = cellList.begin();
			// レイの終点のセル。
			auto rayEndCellIt = rayStartCellIt;
			for (int i = 0; i < skipCellCount; i++) {
				rayEndCellIt++;
			}
			bool isEnd = false;
			while (isEnd == false) {
				// レイの終端のセルまでレイテストをXZ平面で行う。
				Vector3 rayStartPos = (*rayStartCellIt)->cell->GetCenterPosition();
				Vector3 rayEndPos = (*rayEndCellIt)->cell->GetCenterPosition();
				rayStartPos.y = 0.0f;
				rayEndPos.y = 0.0f;
				bool isVisible = true;
				auto cellIt = rayStartCellIt;
				do {
					cellIt++;
					isVisible = isVisible && IsIntercetRayToCell(rayStartPos, rayEndPos, *cellIt);
				} while (cellIt != rayEndCellIt);
				if (isVisible) {
					// 可視
					// 始点から終点が見えるということは、始点～終点までの間のセルは削除できるのでスムースマークを付ける。
					auto cellIt = rayStartCellIt;
					cellIt++;
					while (cellIt != rayEndCellIt) {
						(*cellIt)->isSmooth = true;
						cellIt++;
					}
				}
				// 次。
				rayStartCellIt = rayEndCellIt;
				rayEndCellIt = rayStartCellIt;
				for (int i = 0; i < skipCellCount; i++) {
					rayEndCellIt++;
					if (rayEndCellIt == cellList.end()) {
						isEnd = true;
						break;
					}
				}
			}
			skipCellCount /= 2;
		}
		// スムースフラグが立っているセルを除去していく
		for (auto it = cellList.begin(); it != cellList.end(); it++) {
			if ((*it)->isSmooth) {
				it = cellList.erase(it);
			}
		}
		// 物理オブジェクトとの衝突判定を行い、削除できるセルは削除する。
		if (physicsWorld && cellList.size() > 2) {
			CCapsuleCollider collider;
			collider.Init(agentRadius, agentHeight);
			auto rayStartCellIt = cellList.begin();
			auto rayPrevCellit = rayStartCellIt;
			rayPrevCellit++;
			auto rayEndCellIt = rayPrevCellit;
			rayEndCellIt++;
			while (rayEndCellIt != cellList.end()) {
				Vector3 rayStartPos = (*rayStartCellIt)->pathPoint;
				Vector3 rayEndPos = (*rayEndCellIt)->pathPoint;
				float offset = agentHeight * 0.5f + agentRadius + agentHeight * 0.1f;
				rayStartPos.y += offset;
				rayEndPos.y += offset;
				if (physicsWorld->ConvexSweepTest(collider, rayStartPos, rayEndPos)) {
					// 何かのオブジェクトとぶつかった。
					// 始点から終点の間のセルは削除できない。
					// 次のブロックを見ていく。
					rayStartCellIt = rayEndCellIt;
					rayPrevCellit = rayEndCellIt;
					rayPrevCellit++;
					if (rayPrevCellit == cellList.end()) {
						// 終わり。
						break;
					}
					rayEndCellIt = rayPrevCellit;
					rayEndCellIt++;
				}
				else {
					// ぶつからなかった → 始点と終点の間のセルは除去できる。
					cellList.erase(rayPrevCellit);
					// 次。
					rayPrevCellit = rayEndCellIt;
					rayEndCellIt++;
				}
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
		const Vector3& endPos,
		PhysicsWorld* physicsWorld ,
		float agentRadius,
		float agentHeight)
	{
		// パスをクリア。
		path.Clear();
		// 開始位置に最も近いセルを検索。
		const Cell& startCell = naviMesh.FindNearestCell(startPos);
		// 終了位置に最も近いセルを検索。
		const Cell& endCell = naviMesh.FindNearestCell(endPos);
		// A*を解く時に、セルにいろいろな情報を埋め込むので、情報を埋め込む作業領域を確保。
		m_cellWork.resize(naviMesh.GetNumCell());

		// 作業領域を初期化する。
		for (int cellNo = 0; cellNo < m_cellWork.size(); cellNo++) {
			m_cellWork[cellNo].Init(&naviMesh.GetCell(cellNo));
		}
		// 最後にパスを作るときの座標を設定する。
		// 開始セルは開始座標、終了セルは終了座標。
		// それ以外のセルは、セルの中心座標がCellWork::Init()関数で設定されている。
		m_cellWork[startCell.GetCellNo()].pathPoint = startPos;
		m_cellWork[endCell.GetCellNo()].pathPoint = endPos;
			
		using namespace std;
		vector< CellWork* > openList;
		// 開始セルの作業領域を取得。
		CellWork* crtCell = &m_cellWork.at( startCell.GetCellNo() );
		// 開始セルの移動コストを計算する。
		CalcCost(crtCell->cost, crtCell->costFromStartCell, crtCell, nullptr, &endCell);
		// 開始セルをオープンリストに積む。
		openList.emplace_back(crtCell);
		// セルをオープンリストに積んだ印をつける。
		crtCell->isOpend = true;

		// ここからA*アルゴリズムを解いていくｊ。
		bool isSuccess = false;
		while (!openList.empty()) {
			// オープンリストから最もコストの安いセルを取得。
			crtCell = nullptr;
			float lowestCost = FLT_MAX;
			vector<CellWork*>::iterator lowestCellIt;
			for( auto it = openList.begin(); it != openList.end(); it++ ){

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
				// オープンリストから取り出したセルが終了せるなら経路探索成功。
				isSuccess = true;
				break;
			}
				
			// 開いたセルの隣接セルをオープンリストに積んでいく。
			for (int linkCellNo = 0; linkCellNo < 3; linkCellNo++) {
				Cell* linkCell = crtCell->cell->GetLinkCell(linkCellNo);
				if( linkCell != nullptr){
					// 隣接セルがあったので、コストを計算してオープンリストに積む。
					CellWork* linkCellWork = &m_cellWork.at(linkCell->GetCellNo());

					//隣接セルに移動するコストを計算
					float newCost;
					float newCostFromStartCell;
					CalcCost(newCost, newCostFromStartCell, linkCellWork, crtCell, &endCell);
					
					if (linkCellWork->isClosed == false	) { //隣接セルの調査は終わっていない。
						if (linkCellWork->cost > newCost) {
							// こちらのコストの方が安いので改善する。
							linkCellWork->costFromStartCell = newCostFromStartCell;
							linkCellWork->cost = newCost;
							// 親を設定。
							linkCellWork->parentCell = crtCell;
						}
						if (linkCellWork->isOpend == false) {
							// まだオープンリストに積む。
							linkCellWork->isOpend = true;
							openList.emplace_back(linkCellWork);
						}
					}
						
				}
			}
			// このセルの調査は終了なので、クローズの印をつける。
			crtCell->isClosed = true;
		}
			
		if (isSuccess) {
			// パスを構築。
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
			//
			Smoothing(cellList, physicsWorld, agentRadius, agentHeight);
			
			// ポイントをパスに積んでいく
			for (auto it = cellList.begin(); it != cellList.end(); it++) {
				path.AddPoint((*it)->pathPoint);
			}
			
			// パスを構築する。
			path.Build();
				
		}
	}
}
}


