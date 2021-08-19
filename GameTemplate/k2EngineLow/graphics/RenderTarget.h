#pragma once
#pragma once

//#include "Texture.h"

namespace nsK2EngineLow {
	class GraphicsEngine;

	/// <summary>
	/// �����_�����O�^�[�Q�b�g�B
	/// </summary>
	class RenderTarget : public Noncopyable {
	public:
		/// <summary>
		/// �f�X�g���N�^�B
		/// </summary>
		~RenderTarget();
		/// <summary>
		/// �����_�����O�^�[�Q�b�g�̍쐬�B
		/// </summary>
		/// <param name="w">�����_�����O�^�[�Q�b�g�̕�</param>
		/// <param name="h">�����_�����O�^�[�Q�b�g�̍���</param>
		/// <param name="mipLevel">�~�b�v�}�b�v���x���B0���w�肵���ꍇ�̓~�b�v�}�b�v���T�|�[�g����Ă���GPU�ł�1*1�s�N�Z���܂ł̃~�b�v�}�b�v���쐬�����B</param>
		/// <param name="arraySize">�e�N�X�`���z��̃T�C�Y</param>
		/// <param name="colorFormat">�J���[�o�b�t�@�̃t�H�[�}�b�g�B</param>
		/// <param name="depthStencilFormat">�[�x�X�e���V���o�b�t�@�̃t�H�[�}�b�g�B</param>
		/// <returns>true���Ԃ��Ă�����쐬����</returns>
		bool Create(
			int w,
			int h,
			int mipLevel,
			int arraySize,
			DXGI_FORMAT colorFormat,
			DXGI_FORMAT depthStencilFormat,
			float clearColor[4] = nullptr
		);
		/// <summary>
		/// CPU���̃����_�����O�^�[�Q�b�g�̃f�B�X�N���v�^�n���h�����擾�B
		/// </summary>
		/// <returns></returns>
		D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCpuDescriptorHandle() const
		{
			return m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
		}
		/// <summary>
		/// CPU���̃f�v�X�X�e���V���o�b�t�@�̃f�B�X�N���v�^�n���h�����擾�B
		/// </summary>
		/// <returns></returns>
		D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCpuDescriptorHandle() const
		{
			return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
		}
		/// <summary>
		/// �����_�����O�^�[�Q�b�g�ƂȂ�e�N�X�`�����擾�B
		/// </summary>
		/// <returns></returns>
		Texture& GetRenderTargetTexture()
		{
			return m_renderTargetTexture;
		}
		/// <summary>
		/// �f�v�X�X�e���V���o�b�t�@�����݂��Ă��邩����
		/// </summary>
		/// <returns></returns>
		bool IsExsitDepthStencilBuffer() const
		{
			return m_depthStencilTexture;
		}

		/// <summary>
		/// �����_�����O�^�[�Q�b�g�̕����擾�B
		/// </summary>
		/// <returns></returns>
		int GetWidth() const
		{
			return m_width;
		}
		/// <summary>
		/// �����_�����O�^�[�Q�b�g�̍������擾�B
		/// </summary>
		/// <returns></returns>
		int GetHeight() const
		{
			return m_height;
		}
		/// <summary>
		/// �J���[�o�b�t�@�̃t�H�[�}�b�g���擾�B
		/// </summary>
		/// <returns></returns>
		DXGI_FORMAT GetColorBufferFormat() const
		{
			return m_renderTargetTexture.GetFormat();
		}
		const float* GetRTVClearColor() const
		{
			return m_rtvClearColor;
		}
		float GetDSVClearValue() const
		{
			return m_dsvClearValue;
		}
	private:
		/// <summary>
		/// �f�B�X�N���v�^�q�[�v���쐬�B
		/// </summary>
		/// <param name="ge">�O���t�B�b�N�G���W��</param>
		/// <param name="d3dDevice">D3D�f�o�C�X</param>
		/// <returns></returns>
		bool CreateDescriptorHeap(GraphicsEngine& ge, ID3D12Device5*& d3dDevice);
		/// <summary>
		/// �����_�����O�^�[�Q�b�g�ƂȂ�e�N�X�`�����쐬�B
		/// </summary>
		/// <param name="ge">�O���t�B�b�N�G���W��</param>
		/// <param name="d3dDevice">D3D�f�o�C�X</param>
		/// <param name="w">�e�N�X�`���̕�</param>
		/// <param name="h">�e�N�X�`���̍���</param>
		/// <param name="mipLevel">�~�b�v�}�b�v���x��</param>
		/// <param name="arraySize">�e�N�X�`���z��̃T�C�Y</param>
		/// <param name="format">�e�N�X�`���̃t�H�[�}�b�g</param>
		/// <returns>true���Ԃ��Ă����琬���B</returns>
		bool CreateRenderTargetTexture(
			GraphicsEngine& ge,
			ID3D12Device5*& d3dDevice,
			int w,
			int h,
			int mipLevel,
			int arraySize,
			DXGI_FORMAT format,
			float clearColor[4]
		);
		/// <summary>
		/// �[�x�X�e���V���o�b�t�@�ƂȂ�e�N�X�`�����쐬�B
		/// </summary>
		/// <param name="ge">�O���t�B�b�N�G���W��</param>
		/// <param name="d3dDevice">D3D�f�o�C�X</param>
		/// <param name="w">�e�N�X�`���̕�</param>
		/// <param name="h">�e�N�X�`���̍���</param>
		/// <param name="format">�e�N�X�`���̃t�H�[�}�b�g</param>
		/// <returns>true���Ԃ��Ă����琬��</returns>
		bool CreateDepthStencilTexture(
			GraphicsEngine& ge,
			ID3D12Device5*& d3dDevice,
			int w,
			int h,
			DXGI_FORMAT format);
		/// <summary>
		/// �f�B�X�N���v�^�̍쐬�B
		/// </summary>
		/// <param name="d3dDevice">D3D�f�o�C�X</param>
		/// <returns>true���Ԃ��Ă��`�A�琬���B</returns>
		void CreateDescriptor(ID3D12Device5*& d3dDevice);
		/// <summary>
		/// ���
		/// </summary>
		void Release();
	private:
		Texture m_renderTargetTexture;
		ID3D12Resource* m_renderTargetTextureDx12 = nullptr;//�����_�����O�^�[�Q�b�g�ƂȂ�e�N�X�`���B
		ID3D12Resource* m_depthStencilTexture = nullptr;	//�[�x�X�e���V���o�b�t�@�ƂȂ�e�N�X�`���B
		ID3D12DescriptorHeap* m_rtvHeap = nullptr;	//RTV�p�̃f�B�X�N���v�^�q�[�v�B
		ID3D12DescriptorHeap* m_dsvHeap = nullptr;	//�[�x�X�e���V���o�b�t�@�r���[�̃f�B�X�N���v�^�q�[�v�B
		UINT m_rtvDescriptorSize = 0;						//�t���[���o�b�t�@�̃f�B�X�N���v�^�̃T�C�Y�B
		UINT m_dsvDescriptorSize = 0;						//�[�x�X�e���V���o�b�t�@�̃f�B�X�N���v�^�̃T�C�Y�B
		int m_width = 0;									//�����_�����O�^�[�Q�b�g�̕��B
		int m_height = 0;									//�����_�����O�^�[�Q�b�g�̍����B
		float m_rtvClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	//�����_�����O�^�[�Q�b�g�r���[�̃N���A�J���[�B
		float m_dsvClearValue = 1.0f;							//DSV�̃N���A�J���[�B
	};
}


