#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// �t���[���o�b�t�@�N���X�B
	/// </summary>
	/// <remark>
	/// �t���[���o�b�t�@�Ƃ̓f�B�X�v���C�ɕ\������郁�����̈�̂��Ƃł���B
	/// DirectX12�ł̓X���b�v�`�F�C�����쐬���邱�ƂŃt���[���o�b�t�@���쐬���邱�Ƃ��ł���B
	/// �{�G���W���ł́A��ʂɕ\�����̃o�b�t�@�ƁA���ݕ`�撆�̃o�b�t�@�̂Q���K�v�ɂȂ邽��
	/// �t���[���o�b�t�@�̓_�u���o�b�t�@�ƂȂ��Ă���B
	/// �\�����̊G�̓t�����g�o�b�t�@�A�`�撆�̊G�̓o�b�N�o�b�t�@�ƌď̂����B
	/// FrameBuffer::Present()�֐����Ăяo�����ƂŁA���ݕ`�撆�̃o�b�t�@�̊G���f�B�X�v���C�ɕ\�����邱�Ƃ��ł���B
	/// </remark>
	class FrameBuffer {
	public:
		/// <summary>
		/// �������B
		/// </summary>
		/// <param name="d3dDevice"></param>
		/// <returns></returns>
		bool Init(
			HWND hwnd,
			ID3D12Device5* d3dDevice,
			ID3D12CommandQueue* commandQueue,
			IDXGIFactory4* dxgiFactory,
			UINT frameBufferWidth,
			UINT frameBufferHeight
		);


		~FrameBuffer();

		/// <summary>
		/// ���ݏ������ݐ�ƂȂ��Ă��郌���_�����O�^�[�Q�b�g���擾�B
		/// </summary>
		/// <returns></returns>
		ID3D12Resource* GetCurrentRenderTarget() const
		{
			return m_renderTargets[m_backBufferIndex];
		}
		/// <summary>
		/// ���ݏ������ݐ�ƂȂ��Ă��郌���_�����O�^�[�Q�b�g�r���[�̃f�B�X�N���v�^�n���h�����擾�B
		/// </summary>
		/// <returns></returns>
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTargetViewDescriptorHandle() const
		{
			return m_currentFrameBufferRTVHandle;
		}
		/// <summary>
		/// ���ݏ������ݐ�ƂȂ��Ă���f�v�X�X�e���V���r���[�̃f�B�X�N���v�^�n���h�����擾�B
		/// </summary>
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentDepthStencilViewDescriptorHandle() const
		{
			return m_currentFrameBufferDSVHandle;
		}
		/// <summary>
		/// �r���[�|�[�g���擾�B
		/// </summary>
		/// <returns></returns>
		D3D12_VIEWPORT& GetViewport()
		{
			return m_viewport;
		}
		/// <summary>
		/// �������ݒ��̃o�b�N�o�b�t�@�̓��e���f�B�X�v���C�ɓ]�����܂��B
		/// </summary>
		/// <param name="syncInterval">
		/// �����C���^�[�o���B
		/// �f�B�X�v���C�ɓ]������^�C�~���O���w�肵�܂��B
		/// 0:�������Ȃ��B�����ɑ���B
		/// 1:�����������P��҂��đ���B
		/// 2:�����������Q��҂��đ���B
		/// �@�E
		/// �@�E
		/// �@�E
		/// n:��������������҂��đ���B
		/// </param>
		void Present(UINT syncInterval);
		/// <summary>
		/// �o�b�N�o�b�t�@�����ւ���B
		/// </summary>
		/// <returns></returns>
		void SwapBackBuffer();
		/// <summary>
		/// ���݂̃o�b�N�o�b�t�@�̔ԍ����擾�B
		/// </summary>
		/// <returns></returns>
		UINT GetCurrentBackBufferIndex() const
		{
			return m_backBufferIndex;
		}
		void Release();
	private:
		bool CreateDescriptorHeapForFrameBuffer(ID3D12Device5* d3dDevice);
		bool CreateRTVForFameBuffer(ID3D12Device5* d3dDevice);
		bool CreateDSVForFrameBuffer(ID3D12Device5* d3dDevice, UINT frameBufferWidth, UINT frameBufferHeight);
		bool CreateSwapChain(
			HWND hwnd,
			UINT frameBufferWidth,
			UINT frameBufferHeight,
			IDXGIFactory4* dxgiFactory,
			ID3D12CommandQueue* commandQueue
		);
	private:
		enum { FRAME_BUFFER_COUNT = 2 };						//�t���[���o�b�t�@�̐��B
		ID3D12DescriptorHeap* m_rtvHeap = nullptr;						//�����_�����O�^�[�Q�b�g�r���[�̃f�B�X�N���v�^�q�[�v�B
		ID3D12DescriptorHeap* m_dsvHeap = nullptr;						//�[�x�X�e���V���r���[�̃f�B�X�N���v�^�q�[�v�B
		UINT m_rtvDescriptorSize = 0;		//�t���[���o�b�t�@�̃f�B�X�N���v�^�̃T�C�Y�B
		UINT m_dsvDescriptorSize = 0;		//�[�x�X�e���V���o�b�t�@�̃f�B�X�N���v�^�̃T�C�Y
		ID3D12Resource* m_renderTargets[FRAME_BUFFER_COUNT] = { nullptr };	//�����_�����O�^�[�Q�b�g�B
		ID3D12Resource* m_depthStencilBuffer = nullptr;						//�[�x�X�e���V���o�b�t�@�B
		D3D12_VIEWPORT m_viewport;			//�r���[�|�[�g�B
		D3D12_RECT m_scissorRect;			//�V�U�����O��`�B
		D3D12_CPU_DESCRIPTOR_HANDLE m_currentFrameBufferRTVHandle;		//�����_�����O�^�[�Q�b�g�r���[�̃f�B�X�N���v�^�n���h���B
		D3D12_CPU_DESCRIPTOR_HANDLE m_currentFrameBufferDSVHandle;		//�[�x�X�e���V���r���[�̃f�B�X�N���v�^�n���h��
		UINT m_backBufferIndex = 0;
		IDXGISwapChain3* m_swapChain = nullptr;							//�X���b�v�`�F�C���B
	};
}