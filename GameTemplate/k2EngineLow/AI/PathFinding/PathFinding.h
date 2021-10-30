#pragma once

namespace nsK2EngineLow {
	namespace nsAI {
		class Path;
		class Cell;
		class NaviMesh;
		/// <summary>
		/// �p�X���������B
		/// </summary>
		class PathFinding
		{
		public:
			/// <summary>
			/// �p�X�̌������������s�B
			/// </summary>
			/// <param name="path">���������p�X</param>
			/// <param name="naviMesh">�i�r�Q�[�V�������b�V��</param>
			/// <param name="startPos">�����J�n���W</param>
			/// <param name="endPos">�����I�����W</param>
			void Execute(
				Path& path, 
				const NaviMesh& naviMesh, 
				const Vector3& startPos, 
				const Vector3& endPos
			);
		private:
			struct CellWork {
			public:
				const Cell* m_cell;
				float costFromStartCell;
			};

			std::vector< CellWork > m_cellWork;
		};
	}
}

