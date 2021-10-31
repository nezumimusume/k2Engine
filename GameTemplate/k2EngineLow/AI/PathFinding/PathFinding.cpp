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
			// ����������B
			for (int cellNo = 0; cellNo < m_cellWork.size(); cellNo++) {
				m_cellWork[cellNo].Init(&naviMesh.GetCell(cellNo));
			}
			
			
			using namespace std;
			const int OPEN_CLOSE_LIST_MAX = 1024;
			vector< CellWork* > openList;
			openList.reserve(OPEN_CLOSE_LIST_MAX);
			
			CellWork* crtCell = &m_cellWork.at( startCell.GetCellNo() );
			// �m�[�h�̃R�X�g���v�Z����B
			// �J�n�Z������̈ړ��R�X�g�B�P���ɋ����B
			crtCell->costFromStartCell = 0.0f;
			// �Z�����g�̃R�X�g�̓q���[���X�e�B�b�N�R�X�g�{�J�n�Z������̈ړ��R�X�g
			// ���̃G���W����A*�̃q���[���X�e�B�b�N�R�X�g�͖ڕW�Z���܂ł̋����Ƃ���B
			float distToEndPos = (startCell.GetCenterPosition() - endCell.GetCenterPosition()).Length();
			crtCell->cost = crtCell->costFromStartCell + distToEndPos;
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
						Vector3 t = linkCellWork->cell->GetCenterPosition() - crtCell->cell->GetCenterPosition();
						float newCostFromStartCell = t.Length() + crtCell->costFromStartCell;
						float distToEndPos = (linkCellWork->cell->GetCenterPosition() - endCell.GetCenterPosition()).Length();
						float newCost = newCostFromStartCell + distToEndPos;
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
				// start����end�ւ̌o�H�����������B
				// �p�X���\�z���Ă����B
				vector<Vector3> pointArray;
				// �S�[���Z������X�^�[�g�Z���ɓ��B����܂ŁA�e�����ǂ��Ă����B
				crtCell = &m_cellWork.at(endCell.GetCellNo());
				while (true) {
					if (crtCell->cell == &startCell) {
						// �J�n�Z���ɓ��B�����B
						break;
					}
					pointArray.emplace_back(crtCell->cell->GetCenterPosition());
					crtCell = crtCell->parentCell;
				}
				// �Ō�ɊJ�n�Z����ǉ��B
				pointArray.push_back(startCell.GetCenterPosition());
				// �|�C���g���p�X�ɐς�ł���
				for (auto it = pointArray.rbegin(); it != pointArray.rend(); it++) {
					path.AddPoint(*it);
				}
				// �p�X���\�z����B
				path.Build();
				
			}
		}
	}
}


