#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// tkn�t�@�C��
	/// </summary>
	class TknFile : public Noncopyable {
	public:
		/// <summary>
		/// tkn�t�@�C���̃w�b�_�[
		/// </summary>
		struct SHeader {
			std::uint32_t numCell;	// �Z���̐��B
		};
		/// <summary>
		/// �Z���̃f�[�^�B
		/// </summary>
		struct SCell {
			Vector3 vertexPosition[3];	// �Z���̒��_���W�B
			Vector3 normal;				// �Z���̖@���B
			std::int32_t linkCellNo[3];	// �אڃZ���̔ԍ��B
		};
		/// <summary>
		/// tkn�t�@�C�������[�h����B
		/// </summary>
		/// <param name="filePath">tkn�t�@�C���̃t�@�C���p�X</param>
		/// <returns>false���Ԃ��Ă����烍�[�h���s�B</returns>
		bool Load(const char* filePath);
		/// <summary>
		/// �Z���̐����擾�B
		/// </summary>
		/// <returns></returns>
		int GetNumCell() const
		{
			return m_header.numCell;
		}
		/// <summary>
		/// �Z�����擾
		/// </summary>
		/// <param name="cellNo">�Z���ԍ�</param>
		/// <returns></returns>
		const SCell& GetCell(int cellNo) const
		{
			return m_cellArray.at(cellNo);
		}
	private:
		SHeader m_header;				// �w�b�_�B
		std::vector<SCell> m_cellArray;	// �Z���̔z��B
	};
}