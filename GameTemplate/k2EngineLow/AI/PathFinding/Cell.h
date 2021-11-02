#pragma once

namespace nsK2EngineLow {
	class TknFile;
	namespace nsAI {
		/// <summary>
		/// �i�r���b�V���̃Z��
		/// </summary>
		class Cell {
		public:
			/// <summary>
			/// ���_���W��ݒ�B
			/// </summary>
			/// <param name="vertNo">���_�ԍ�</param>
			/// <param name="pos">���W</param>
			void SetVertexPosition(int vertNo, const Vector3& pos)
			{
				K2_ASSERT(vertNo < 3, "Invalid vertNo");
				m_vertexPosition[vertNo] = pos;
			}
			/// <summary>
			/// �@����ݒ�B
			/// </summary>
			/// <param name="normal">�@��</param>
			void SetNormal(const Vector3& normal)
			{
				m_normal = normal;
			}
			/// <summary>
			/// �אڃZ������ݒ�B
			/// </summary>
			/// <param name="linkNo">�אڔԍ��B0�`2�B</param>
			/// <param name="linkCell">�אڃZ��</param>
			void SetLinkCell(int linkNo, Cell* linkCell)
			{
				K2_ASSERT(linkNo < 3, "Invalid linkNo");
				m_linkCell[linkNo] = linkCell;
			}
			/// <summary>
			/// �Z���̒��S���W��ݒ�B
			/// </summary>
			/// <param name="centerPos"></param>
			void SetCenterPosition(const Vector3& centerPos)
			{
				m_centerPos = centerPos;
			}
			/// <summary>
			/// �Z���̒��S���W���擾�B
			/// </summary>
			/// <returns></returns>
			const Vector3& GetCenterPosition()const
			{
				return m_centerPos;
			}
			/// <summary>
			/// �Z���ԍ���ݒ�B
			/// </summary>
			/// <param name="no">�ԍ�</param>
			void SetCellNo(int no)
			{
				m_no = no;
			}
			/// <summary>
			/// �Z���ԍ����擾�B
			/// </summary>
			/// <returns></returns>
			int GetCellNo() const
			{
				return m_no;
			}
			/// <summary>
			/// �אڃZ�����擾�B
			/// </summary>
			/// <param name="linkCellNo"></param>
			/// <returns></returns>
			Cell* GetLinkCell(int linkCellNo)const
			{
				return m_linkCell[linkCellNo];
			}
			/// <summary>
			/// �Z���̒��_���W���擾����B
			/// </summary>
			/// <param name="vertNo">���_�ԍ�</param>
			/// <returns>���_���W</returns>
			const Vector3& GetVertexPosition(int vertNo) const
			{
				return m_vertexPosition[vertNo];
			}
		private:
			Vector3 m_vertexPosition[3];		// �Z���̒��_���W
			Vector3 m_normal;					// �@��
			Vector3 m_centerPos;				// ���S���W�B
			Cell* m_linkCell[3] = { nullptr };	// �אڃZ��
			int m_no = 0;						// �Z���ԍ�
		};
	}
}

