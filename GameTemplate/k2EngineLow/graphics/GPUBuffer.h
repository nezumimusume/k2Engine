#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// GPU�o�b�t�@�N���X
	/// </summary>
	/// <remarks>
	/// �ėp�I��GPU�o�b�t�@�N���X�ł��B
	/// �e�N�X�`���A�X�g���N�`���[�h�o�b�t�@�A���_�o�b�t�@���ׂẴo�b�t�@���쐬���邱�Ƃ��ł��܂��B
	/// </remarks>
	class GPUBuffer : public IUnorderAccessResrouce {
	public:
		/// <summary>
		/// �f�X�g���N�^�B
		/// </summary>
		~GPUBuffer();

		/// <summary>
		/// �������B
		/// </summary>
		/// <param name="desc"></param>
		void Init(D3D12_RESOURCE_DESC desc);
		/// <summary>
		/// UAV�Ƃ��ăf�B�X�N���v�^�q�[�v�ɓo�^
		/// </summary>
		/// <param name="cpuHandle">�o�^����f�B�X�N���v�^�q�[�v��CPU�n���h��</param>
		/// <param name="bufferNo">�o�b�t�@�ԍ�</param>
		void RegistUnorderAccessView(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, int bufferNo) override;
		/// <summary>
		/// SRV�Ƃ��ăf�B�X�N���v�^�q�[�v�ɓo�^�B
		/// </summary>
		/// <param name="descriptorHandle">�o�^����f�B�X�N���v�^�q�[�v��CPU�n���h��</param>
		/// <param name="bufferNo">�o�b�t�@�ԍ�</param>
		void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo) override;

		ID3D12Resource* Get()
		{
			return m_buffer;
		}
	private:
		/// <summary>
		/// ����B
		/// </summary>
		void Release();
	private:
		ID3D12Resource* m_buffer = nullptr;
		D3D12_RESOURCE_DESC m_desc;
	};
}