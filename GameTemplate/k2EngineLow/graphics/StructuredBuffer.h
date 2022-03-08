#pragma once

namespace nsK2EngineLow {

	/// <summary>
	/// �\�����o�b�t�@
	/// </summary>
	class StructuredBuffer : public IShaderResource {
	public:
		/// <summary>
		/// �f�X�g���N�^�B
		/// </summary>
		~StructuredBuffer();
		/// <summary>
		/// �\�����o�b�t�@���������B
		/// </summary>
		/// <param name="sizeOfElement">�G�������g�̃T�C�Y�B</param>
		/// <param name="numElement">�G�������g�̐��B</param>
		/// <param name="initData">�����f�[�^�B</param>
		/// <param name="isDoubleBuffer">�����Ń_�u���o�b�t�@������H�B</param>
		void Init(int sizeOfElement, int numElement, void* initData, bool isDoubleBuffer = true);
		/// <summary>
		/// ���_�o�b�t�@���g���č\�����o�b�t�@������������B
		/// </summary>
		/// <param name="vb"></param>
		/// <param name="isUpdateByCPU"></param>
		void Init(const VertexBuffer& vb, bool isUpdateByCPU);
		
		/// <summary>
		/// SRV�ɓo�^�B
		/// </summary>
		/// <param name="descriptorHandle"></param>
		void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo) override;
		/// <summary>
		/// �\�����o�b�t�@�̓��e���X�V�B
		/// </summary>
		/// <param name="data"></param>
		void Update(void* data);
		/// <summary>
		/// ����������Ă��邩����B
		/// </summary>
		/// <returns></returns>
		bool IsInited() const
		{
			return m_isInited;
		}
		/// <summary>
		/// D3D���\�[�X���擾�B
		/// </summary>
		/// <returns></returns>
		ID3D12Resource* GetD3DResoruce();
	private:
		/// <summary>
		/// ��������B
		/// </summary>
		void Release();
		/// <summary>
		/// �o�b�N�o�b�t�@�̔ԍ����擾�B
		/// </summary>
		/// <returns></returns>
		int GetBackBufferNo() const;
	private:
		ID3D12Resource* m_buffersOnGPU[2] = { nullptr };
		void* m_buffersOnCPU[2] = { nullptr };		//CPU������A�N�Z�X�ł��邷��X�g���N�`���o�b�t�@�̃A�h���X�B
		int m_numElement = 0;						//�v�f���B
		int m_sizeOfElement = 0;					//�G�������g�̃T�C�Y�B
		bool m_isInited = false;					//�������ς݁H
		bool m_isDoubleBuffer = true;
	};
}