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
			/// <param name="physicsWorld">
			/// �������[���h�B
			/// �������[���h���w�肳��Ă���ƁA�������[���h�ɔz�u����Ă���I�u�W�F�N�g��
			/// �Փ˔�����s���A�p�X�̃X���[�W���O�����P���܂��B
			/// ���̏Փ˔���ł́AagentRadius��agentHeight�̃p�����[�^���g�p����܂��B
			/// </param>
			/// <param name="agentRadius">
			/// AI�G�[�W�F���g�̔��a�B�G�[�W�F���g�̃J�v�Z���R���C�_�[���쐬����̂ɗ��p����܂��B
			/// </param>
			/// <param name="agentHeight">
			/// AI�G�[�W�F���g�̍����G�[�W�F���g�̃J�v�Z���R���C�_�[���쐬����̂ɗ��p����܂��B
			/// </param>
			void Execute(
				Path& path, 
				const NaviMesh& naviMesh, 
				const Vector3& startPos, 
				const Vector3& endPos,
				PhysicsWorld* physicsWorld = nullptr,
				float agentRadius = 50.0f,
				float agentHeight = 200.0f
			);
		private:
			struct CellWork {

				void Init(const Cell* cell);
				
				const Cell* cell;
				CellWork* parentCell;	// �e�̃Z���B
				float costFromStartCell;
				Vector3 pathPoint;
				float cost;				// �ړ��R�X�g
				bool isOpend;			// �J���ꂽ�H
				bool isClosed;			// ����ꂽ�H
				bool isSmooth;			// �X���[�X�����H
			};
		private:
		
			/// <summary>
			/// ���̃Z���Ɉړ�����R�X�g���v�Z
			/// </summary>
			/// <returns></returns>
			void CalcCost(
				float& totalCost, 
				float& costFromStartCell, 
				const CellWork* nextCell, 
				const CellWork* prevCell, 
				const Cell* endCell);
			/// <summary>
			/// �X���[�W���O
			/// </summary>
			/// <param name="cellList"></param>
			void Smoothing(
				std::list<CellWork*>& cellList,
				PhysicsWorld* physicsWorld,
				float agentRadius,
				float agentHeight
			);
			/// <summary>
			/// ���C�ƃZ���̌�������
			/// </summary>
			bool IsIntercetRayToCell(Vector3 rayStartPos, Vector3 rayEndPos, CellWork*) const;
		private:
			

			std::vector< CellWork > m_cellWork;
		};
	}
}

