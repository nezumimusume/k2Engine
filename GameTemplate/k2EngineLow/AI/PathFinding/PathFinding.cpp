#include "k2EngineLowPreCompile.h"
#include "AI/PathFinding/PathFinding.h"
#include "AI/PathFinding/NaviMesh.h"
#include "AI/PathFinding/Path.h"

namespace nsK2EngineLow {
namespace nsAI {
	
	void PathFinding::Smoothing(std::list<CellWork*>& cellList) 
	{
		// �p�X�̉�������s���āA�s�v�ȃZ�������O���Ă����B
		if (cellList.size() < 3) {
			// �Z���̐���3�ȉ��Ȃ�X���[�W���O����K�v�Ȃ��B
			return;
		}

		// �Z����2���傫����΁A�p�X�̉�������s���āA�s�v�ȃZ�������O���Ă����B
		// ���C�̎n�_�ƂȂ�Z���B
		auto rayStartCellIt = cellList.begin();
		// ���C�̏I�_�̎�O�̃Z���B
		auto prevRayEndCellIt = rayStartCellIt;
		prevRayEndCellIt++;
		// ���C�̏I�_�̃Z���B
		auto rayEndCellIt = prevRayEndCellIt;
		rayEndCellIt++;

		auto endCellIt = cellList.end();
		endCellIt--;
		auto prevEndCellIt = endCellIt;
		prevEndCellIt--;

		while (true) {
			// ���C�̏I�[�̃Z���܂Ń��C�e�X�g���s���B
			Vector3 rayStartPos = (*rayStartCellIt)->cell->GetCenterPosition();
			Vector3 rayEndPos = (*rayEndCellIt)->cell->GetCenterPosition();

			// todo �����ɉ��������������B

			if (false) {
				// ��
				// �n�_����I�_��������Ƃ������Ƃ́A�I�_�̎�O�̃Z���͍폜�ł���B
				cellList.erase(prevRayEndCellIt);
				// ���B
				prevRayEndCellIt = rayEndCellIt;
				rayEndCellIt++;
			}
			else {
				// �����Ȃ��Ƃ������Ƃ́A�I�[�Z���܂ł̃X���[�W���O������ȏ�s�����Ƃ͂ł��Ȃ��B
				if (rayEndCellIt == endCellIt
					|| rayEndCellIt == prevEndCellIt
				) {
					// �I���B
					break;
				}
				// ���݂̏I�[�Z����
				rayStartCellIt = rayEndCellIt;
				prevRayEndCellIt = rayStartCellIt;
				prevRayEndCellIt++;
				rayEndCellIt = prevRayEndCellIt;
				rayEndCellIt++;
			}
		}
	}
	/// <summary>
	/// ���̃Z���Ɉړ�����R�X�g���v�Z
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
			// ���O�̃Z�����Ȃ��B
			costFromStartCell = 0.0f;
		}
		else {
			// ���O�̃Z��������B
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
		// ����������B
		for (int cellNo = 0; cellNo < m_cellWork.size(); cellNo++) {
			m_cellWork[cellNo].Init(&naviMesh.GetCell(cellNo));
		}
			
			
		using namespace std;
		const int OPEN_CLOSE_LIST_MAX = 1024;
		vector< CellWork* > openList;
		openList.reserve(OPEN_CLOSE_LIST_MAX);
			
		CellWork* crtCell = &m_cellWork.at( startCell.GetCellNo() );
		// �J�n�Z���̈ړ��R�X�g���v�Z����B
		CalcCost(crtCell->cost, crtCell->costFromStartCell, crtCell, nullptr, &endCell);
		crtCell->isOpend = true;

		// �J�n�Z�����I�[�v�����X�g�ɐςށB
		openList.emplace_back(crtCell);

		bool isSuccess = false;
		while (!openList.empty()) {
			// �I�[�v�����X�g����ł��R�X�g�̈����Z�����擾�B
			crtCell = nullptr;
			float lowestCost = FLT_MAX;
			vector<CellWork*>::iterator lowestCellIt;
			for( auto it = openList.begin(); it != openList.end(); it++ ){
				if ((*it)->isClosed) {
					int hoge = 0;
				}
				if (lowestCost > (*it)->cost) {
					// ���̃Z���̕��������B
					lowestCost = (*it)->cost;
					crtCell = (*it);
					lowestCellIt = it;
				}
			}
			// ���o�����Z�����I�[�v�����X�g����폜�B
			openList.erase(lowestCellIt);
			if (crtCell->cell == &endCell) {
				// �o�H�T������
				isSuccess = true;
				break;
			}
				
			// �אڃZ�����I�[�v�����X�g�ɐς�ł����B
			for (int linkCellNo = 0; linkCellNo < 3; linkCellNo++) {
				Cell* linkCell = crtCell->cell->GetLinkCell(linkCellNo);
				if( linkCell != nullptr){
					// �אڃZ�����������̂ŁA�R�X�g���v�Z���ăI�[�v�����X�g�ɐςށB
					CellWork* linkCellWork = &m_cellWork.at(linkCell->GetCellNo());

					// ���݂̃Z������אڃZ���Ɉړ�����R�X�g���v�Z
					float newCost;
					float newCostFromStartCell;
					CalcCost(newCost, newCostFromStartCell, linkCellWork, crtCell, &endCell);
					
					if (linkCellWork->isClosed == false	) {
						if (linkCellWork->cost > newCost) {
							// ������̃R�X�g�̕��������̂ŉ��P����B
							linkCellWork->costFromStartCell = newCostFromStartCell;
							linkCellWork->cost = newCost;
							// �e��ݒ�B
							linkCellWork->parentCell = crtCell;
						}
						if (linkCellWork->isOpend == false) {
							// �܂��J���Ă��Ȃ��Ȃ�
							linkCellWork->isOpend = true;
							openList.emplace_back(linkCellWork);
						}
							
					}
						
				}
			}
			// ���̃Z���̒����͏I���Ȃ̂ŁA�N���[�Y�̂��邵������B
			crtCell->isClosed = true;
		}
			
		if (isSuccess) {
			list<CellWork*> cellList;
			crtCell = &m_cellWork.at(endCell.GetCellNo());
			while (true) {
				if (crtCell->cell == &startCell) {
					// �J�n�Z���ɓ��B�����B
					// �Ō�ɊJ�n�Z����ςށB
					cellList.emplace_back(crtCell);
					break;
				}
				cellList.emplace_back(crtCell);
				crtCell = crtCell->parentCell;
			}
			// ���]������B
			std::reverse(cellList.begin(), cellList.end());
			Smoothing(cellList);
			
			// �|�C���g���p�X�ɐς�ł���
			for (auto it = cellList.begin(); it != cellList.end(); it++) {
				path.AddPoint((*it)->cell->GetCenterPosition());
			}
			// �p�X���\�z����B
			path.Build();
				
		}
	}
}
}


