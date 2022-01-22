#pragma once

namespace nsK2EngineLow {
	class RWStructuredBuffer;
	/// <summary>
	/// ���_�o�b�t�@
	/// </summary>
	class VertexBuffer : public Noncopyable {

	public:
		/// <summary>
		/// �f�X�g���N�^�B
		/// </summary>
		~VertexBuffer();
		/// <summary>
		/// �������B
		/// </summary>
		/// <param name="size">���_�o�b�t�@�̃T�C�Y</param>
		/// <param name="stride">���_�o�b�t�@�̃X�g���C�h�B��̒��_�̃T�C�Y</param>
		void Init(int size, int stride);
		/// <summary>
		/// RW�X�g���N�`���[�h�o�b�t�@���珉��������B
		/// </summary>
		/// <param name="rwStructuredBuffer"></param>
		void Init(RWStructuredBuffer& rwStructuredBuffer);
		/// <summary>
		/// ���_�f�[�^�𒸓_�o�b�t�@�ɃR�s�[�B
		/// </summary>
		/// <param name="srcVertices">�R�s�[���̒��_�f�[�^�B</param>
		void Copy(void* srcVertices);
		/// <summary>
		/// ���_�o�b�t�@�r���[���擾�B
		/// </summary>
		/// <returns>�r���[</returns>
		const D3D12_VERTEX_BUFFER_VIEW& GetView() const
		{
			return m_vertexBufferView;
		}
		/// <summary>
		/// ���_�o�b�t�@�̃X�g���C�h(�P�ʁF�o�C�g)���擾�B
		/// </summary>
		/// <remarks>
		/// �X�g���C�h�Ƃ����̂́A�P�v�f�̃T�C�Y�̂��ƁB
		/// �����ł́A��̒��_�̃T�C�Y���擾���邱�ƂɂȂ�B
		/// </remarks>
		/// <returns></returns>
		UINT GetStrideInBytes() const
		{
			return m_vertexBufferView.StrideInBytes;
		}
		/// <summary>
		/// ���_�o�b�t�@�̃T�C�Y(�P�ʁF�o�C�g)���擾�B
		/// </summary>
		/// <returns></returns>
		UINT GetSizeInBytes() const
		{
			return m_vertexBufferView.SizeInBytes;
		}
		/// <summary>
		/// ID3D12Resource�̃A�h���X���擾���܂��B
		/// </summary>
		/// <returns></returns>
		ID3D12Resource* GetID3DResourceAddress() const
		{
			return m_vertexBuffer;
		}
		/// <summary>
		/// ���_�����擾�B
		/// </summary>
		/// <returns></returns>
		int GetNumVertex() const
		{
			return GetSizeInBytes() / GetStrideInBytes();
		}
	private:
		void Release();
	private:
		ID3D12Resource* m_vertexBuffer = nullptr;		//���_�o�b�t�@�B
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;	//���_�o�b�t�@�r���[�B
	};
}