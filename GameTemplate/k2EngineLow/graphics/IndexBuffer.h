#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// �C���f�b�N�X�o�b�t�@�B
	/// </summary>
	/// <remarks>
	/// </remarks>
	class IndexBuffer : public Noncopyable {
	public:
		/// <summary>
		/// �f�X�g���N�^�B
		/// </summary>
		~IndexBuffer();
		/// <summary>
		/// �������B
		/// </summary>
		/// <param name="size">�C���f�b�N�X�o�b�t�@�̃T�C�Y�B</param>
		/// <param name="stride">�X�g���C�h�B</param>
		void Init(int size, int stride);
		/// <summary>
		/// �C���f�b�N�X�f�[�^���C���f�b�N�X�o�b�t�@�ɃR�s�[�B
		/// </summary>
		/// <param name="srcIndecies">�R�s�[���̃C���f�b�N�X�f�[�^�B</param>
		void Copy(uint16_t* srcIndecies);
		/// <summary>
		/// �C���f�b�N�X�f�[�^���C���f�b�N�X�o�b�t�@�ɃR�s�[�B
		/// </summary>
		/// <param name="srcIndecies">�R�s�[���̃C���f�b�N�X�f�[�^�B</param>
		void Copy(uint32_t* srcIndecies);
		/// <summary>
		/// �C���f�b�N�X�o�b�t�@�r���[���擾�B
		/// </summary>
		/// <returns></returns>
		const D3D12_INDEX_BUFFER_VIEW& GetView() const
		{
			return m_indexBufferView;
		}
		/// <summary>
		/// �C���f�b�N�X�̐����擾�B
		/// </summary>
		/// <returns>�C���f�b�N�X�̐��B</returns>
		int GetCount() const
		{
			return m_count;
		}
		/// <summary>
		/// �C���f�b�N�X�o�b�t�@�̃X�g���C�h���擾�B
		/// </summary>
		/// �X�g���C�h�Ƃ����̂́A�P�v�f�̃T�C�Y�̂��ƁB
		/// �����ł́A��̃C���f�b�N�X�̃T�C�Y���擾���邱�ƂɂȂ�B
		/// 2��4���Ԃ��Ă��܂��B
		/// <returns></returns>
		UINT GetStrideInBytes() const
		{
			return static_cast<UINT>(m_strideInBytes);
		}
		/// <summary>
		/// �C���f�b�N�X�o�b�t�@�̃T�C�Y(�P�ʁF�o�C�g)���擾�B
		/// </summary>
		/// <returns></returns>
		UINT GetSizeInBytes() const
		{
			return static_cast<UINT>(m_sizeInBytes);
		}
		/// <summary>
		/// ID3D12Resource�̃A�h���X���擾���܂��B
		/// </summary>
		/// <returns></returns>
		ID3D12Resource* GetID3DResourceAddress() const
		{
			return m_indexBuffer;
		}
	private:

		/// <summary>
		/// ���
		/// </summary>
		void Release();
	private:
		ID3D12Resource* m_indexBuffer = nullptr;	//�C���f�b�N�X�o�b�t�@�B
		D3D12_INDEX_BUFFER_VIEW m_indexBufferView;	//�C���f�b�N�X�o�b�t�@�r���[�B
		int m_count = 0;							//�C���f�b�N�X�̐��B
		int m_strideInBytes = 0;					//�X�g���C�h(�P�ʁF�o�C�g)�B
		int m_sizeInBytes = 0;						//�T�C�Y(�P�ʁF�o�C�g)�B
	};
}