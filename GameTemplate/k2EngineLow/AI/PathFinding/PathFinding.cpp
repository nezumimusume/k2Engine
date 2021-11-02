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
		// ���C���v�Z����B
		Vector3 ray = rayEndPos - rayStartPos;

		bool isVisible = false;

		// ���Ƃ������Ƃ́ArayStartPos����rayEndPos�Ɍ������ĐL�т�������A�Z���̂ǂꂩ�P�̃G�b�W�ƌ������Ă���Ƃ������ƁB
		const int vertNo[3][2] = {
			{0, 1},
			{1, 2},
			{2, 0},
		};
		const Cell* currentCell = currentCellWork->cell;
		for (int edgeNo = 0; edgeNo < 3; edgeNo++) {
			const Vector3& p0 = currentCell->GetVertexPosition(vertNo[edgeNo][0]);
			const Vector3& p1 = currentCell->GetVertexPosition(vertNo[edgeNo][1]);
			// �܂��͖��������Ƃ��Č������Ă��邩����B
			Vector3 p0ToStartPos = rayStartPos - p0;
			p0ToStartPos.y = 0.0f;
			Vector3 p0ToEndPos = rayEndPos - p0;
			p0ToEndPos.y = 0.0f;

			// p0ToStartPos��p0ToEndPos�𐳋K������B
			Vector3 p0ToStartPosNorm = p0ToStartPos;
			p0ToStartPosNorm.Normalize();
			Vector3 p0ToEndPosNorm = p0ToEndPos;
			p0ToEndPosNorm.Normalize();

			if (p0ToStartPosNorm.Dot(p0ToEndPosNorm) <= 0.0f) {
				// �������Ă���B
				// �����Č�_�����߂�B
				// �܂��́AXZ���ʂŃ��C�ɐ����Ȑ��������߂�B
				Vector3 rayNorm = ray;
				rayNorm.Normalize();
				Vector3 rayTangent;
				rayTangent.Cross(rayNorm, g_vec3AxisY);
				rayTangent.Normalize();
				float t0 = fabsf(Dot(rayTangent, p0ToStartPos));
				float t1 = fabsf(Dot(rayTangent, p0ToEndPos));
				// �n�_�����_�܂ł̔䗦�����߂�B
				float rate = t0 / (t0 + t1);
				Vector3 hitPos = Math::Lerp(rate, rayStartPos, rayEndPos);
				// �����Č�_��������ɂ��邩���ׂ�B
				Vector3 rsToHitPos = hitPos - rayStartPos;
				Vector3 reToHitPos = hitPos - rayEndPos;
				rsToHitPos.Normalize();
				reToHitPos.Normalize();
				if (rsToHitPos.Dot(reToHitPos) <= 0.0f) {
					// �������Ă���ꍇ�͂��̃x�N�g�����t�����ɂȂ�͂��B
					// �������Ă���B
					isVisible = true;
					// ��_���L������B
					currentCellWork->pathPointWork = hitPos;
					break;
				}
			}
		}

		return isVisible;
	}
	void PathFinding::Smoothing(std::list<CellWork*>& cellList) 
	{
		// �p�X�̉�������s���āA�s�v�ȃZ�������O���Ă����B
		if (cellList.size() < 3) {
			// �Z���̐���3�ȉ��Ȃ�X���[�W���O����K�v�Ȃ��B
			return;
		}
		
		int skipCellCount = cellList.size()-1;
		while (skipCellCount > 2) {
			// �Z���̐����R�ȏ�Ȃ�A�p�X�̉�������s���āA�s�v�ȃZ�������O���Ă����B
			// ���C�̎n�_�ƂȂ�Z���B
			auto rayStartCellIt = cellList.begin();
			// ���C�̏I�_�̃Z���B
			auto rayEndCellIt = rayStartCellIt;
			for (int i = 0; i < skipCellCount; i++) {
				rayEndCellIt++;
			}
			bool isEnd = false;
			while (isEnd == false) {
				// ���C�̏I�[�̃Z���܂Ń��C�e�X�g��XZ���ʂōs���B
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
					// ��
					// �n�_����I�_��������Ƃ������Ƃ́A�n�_�`�I�_�܂ł̊Ԃ̃Z���͍폜�ł���̂ŃX���[�X�}�[�N��t����B�B
					//(*rayEndCellIt)->pathPoint = (*rayEndCellIt)->pathPointWork;
					auto cellIt = rayStartCellIt;
					cellIt++;
					while (cellIt != rayEndCellIt) {
						(*cellIt)->isSmooth = true;
						cellIt++;
					}
				}
				// ���B
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

			skipCellCount--;
		}
		// �X���[�X�t���O�������Ă���Z�����������Ă���
		for (auto it = cellList.begin(); it != cellList.end(); it++) {
			if ((*it)->isSmooth) {
				it = cellList.erase(it);
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
		m_cellWork[endCell.GetCellNo()].pathPoint = endPos;
			
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
				path.AddPoint((*it)->pathPoint);
			}
			// �p�X���\�z����B
			path.Build();
				
		}
	}
}
}


