#pragma once

#include "AI/PathFinding/Cell.h"
#include "geometry/BSP.h"

namespace nsK2EngineLow {
	class TknFile;
	namespace nsAI {
		class Cell;
		/// <summary>
		/// �i�r�Q�[�V�������b�V���N���X�B
		/// </summary>
		/// <remark>
		/// 3dsMax����nvmExporter���g�p���ďo�͂��ꂽ�f�[�^�����Ƃɍ\�z����܂��B
		/// </remark>
		class NaviMesh{
		public:
			/// <summary>
			/// tkn�t�@�C������i�r�Q�[�V�������b�V�����\�z����B
			/// </summary>
			/// <param name="tknFilePath">tkn�t�@�C���̃t�@�C���p�X</param>
			void Init(const char* tknFilePath);
			/// <summary>
			/// �w�肵�����W�ɍł��߂��Z������������B
			/// </summary>
			/// <param name="pos">���W</param>
			/// <returns>�Z��</returns>
			const Cell& FindNearestCell(const Vector3& pos) const;
			/// <summary>
			/// �Z���̐����擾�B
			/// </summary>
			/// <returns></returns>
			int GetNumCell() const
			{
				return static_cast<int>(m_cellArray.size());
			}
			/// <summary>
			/// �Z�����擾�B
			/// </summary>
			/// <param name="cellNo"></param>
			/// <returns></returns>
			const Cell& GetCell(int cellNo) const
			{
				return m_cellArray.at(cellNo);
			}
		private:
			std::vector< Cell > m_cellArray;	// �Z���̔z��
			BSP m_cellCenterPosBSP;				// �Z���̒��S���W�ō\�z���ꂽBSP
		};
	}
}

