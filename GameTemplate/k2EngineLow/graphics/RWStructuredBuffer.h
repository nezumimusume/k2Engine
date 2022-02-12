#pragma once

namespace nsK2EngineLow {
	class VertexBuffer;
	class IndexBuffer;

	/// <summary>
	/// �\�����o�b�t�@
	/// </summary>
	class RWStructuredBuffer : public IUnorderAccessResrouce {
	public:

		/// <summary>
		/// �f�X�g���N�^�B
		/// </summary>
		~RWStructuredBuffer();
		/// <summary>
		/// �\�����o�b�t�@���������B
		/// </summary>
		/// <param name="sizeOfElement">�G�������g�̃T�C�Y�B</param>
		/// <param name="numElement">�G�������g�̐��B</param>
		/// <param name="initData">�����f�[�^�B</param>
		/// <param name="isAccessCPU">CPU����A�N�Z�X����H</param>
		void Init(int sizeOfElement, int numElement, void* initData, bool isAccessCPU);
		/// <summary>
		/// ���_�o�b�t�@���g�p���č\�����o�b�t�@���������B
		/// </summary>
		/// <param name="vb">���_�o�b�t�@</param>
		/// <param name="isUpdateByCPU">
		/// CPU���Œ��_�o�b�t�@���X�V���邩�ǂ����B
		/// CPU���ōX�V����ꍇ�A�����œn���ꂽ���_�o�b�t�@�̕������������܂��B
		/// ���̂��߁A�������̎g�p�ʂ͂R�{�ɂȂ�܂��B
		/// </param>
		void Init(const VertexBuffer& vb, bool isUpdateByCPU);
		/// <summary>
		/// �C���f�b�N�X�o�b�t�@�𗘗p���č\�����o�b�t�@���������B
		/// </summary>
		/// <param name="ib">�C���f�b�N�X�o�b�t�@</param>
		/// <param name="isUpdateByCPU">
		/// CPU���ŃC���f�b�N�X�o�b�t�@���X�V���邩�ǂ����B
		/// CPU���ōX�V����ꍇ�A�����œn���ꂽ�C���f�b�N�X�o�b�t�@�̕������������܂��B
		/// ���̂��߁A�������̎g�p�ʂ͂R�{�ɂȂ�܂��B
		/// </param>
		void Init(const IndexBuffer& ib, bool isUpdateByCPU);
		/// <summary>
		/// UAV�ɓo�^�B
		/// </summary>
		/// <param name=""></param>
		/// <param name="bufferNo"></param>
		void RegistUnorderAccessView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo) override;
		/// <summary>
		/// SRV�ɓo�^�B
		/// </summary>
		/// <param name="descriptorHandle"></param>
		/// <param name="bufferNo"></param>
		void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo) override;
		/// <summary>
		/// ����������Ă��邩����B
		/// </summary>
		/// <returns></returns>
		bool IsInited() const
		{
			return m_isInited;
		}
		/// <summary>
		/// CPU����A�N�Z�X�\�ȃ��\�[�X���擾����B
		/// </summary>
		/// <returns></returns>
		void* GetResourceOnCPU();
		/// <summary>
		/// �o�b�t�@�T�C�Y���擾�B
		/// </summary>
		/// <returns></returns>
		int GetSize() const
		{
			return m_sizeOfElement * m_numElement;
		}
		/// <summary>
		/// 1�v�f�̃T�C�Y���擾�B
		/// </summary>
		/// <returns></returns>
		int GetStride() const
		{
			return m_sizeOfElement;
		}
		ID3D12Resource* GetD3DResoruce();
	private:
		/// <summary>
		/// ����B
		/// </summary>
		void Release();
	private:
		ID3D12Resource* m_buffersOnGPU[2] = { nullptr };
		void* m_buffersOnCPU[2] = { nullptr };		//CPU������A�N�Z�X�ł��邷��X�g���N�`���o�b�t�@�̃A�h���X�B
		int m_numElement = 0;				// �v�f���B
		int m_sizeOfElement = 0;			// �G�������g�̃T�C�Y�B
		bool m_isInited = false;			// �������ς݁H
		bool m_isAccessCPU = false;			// CPU����A�N�Z�X����H
	};
}