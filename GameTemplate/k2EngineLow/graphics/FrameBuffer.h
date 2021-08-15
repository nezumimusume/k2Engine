#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// フレームバッファクラス。
	/// </summary>
	/// <remark>
	/// フレームバッファとはディスプレイに表示されるメモリ領域のことである。
	/// DirectX12ではスワップチェインを作成することでフレームバッファを作成することができる。
	/// 本エンジンでは、画面に表示中のバッファと、現在描画中のバッファの２つが必要になるため
	/// フレームバッファはダブルバッファとなっている。
	/// 表示中の絵はフロントバッファ、描画中の絵はバックバッファと呼称される。
	/// FrameBuffer::Present()関数を呼び出すことで、現在描画中のバッファの絵をディスプレイに表示することができる。
	/// </remark>
	class FrameBuffer {
	public:
		/// <summary>
		/// 初期化。
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
		/// 現在書き込み先となっているレンダリングターゲットを取得。
		/// </summary>
		/// <returns></returns>
		ID3D12Resource* GetCurrentRenderTarget() const
		{
			return m_renderTargets[m_backBufferIndex];
		}
		/// <summary>
		/// 現在書き込み先となっているレンダリングターゲットビューのディスクリプタハンドルを取得。
		/// </summary>
		/// <returns></returns>
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTargetViewDescriptorHandle() const
		{
			return m_currentFrameBufferRTVHandle;
		}
		/// <summary>
		/// 現在書き込み先となっているデプスステンシルビューのディスクリプタハンドルを取得。
		/// </summary>
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentDepthStencilViewDescriptorHandle() const
		{
			return m_currentFrameBufferDSVHandle;
		}
		/// <summary>
		/// ビューポートを取得。
		/// </summary>
		/// <returns></returns>
		D3D12_VIEWPORT& GetViewport()
		{
			return m_viewport;
		}
		/// <summary>
		/// 書き込み中のバックバッファの内容をディスプレイに転送します。
		/// </summary>
		/// <param name="syncInterval">
		/// 同期インターバル。
		/// ディスプレイに転送するタイミングを指定します。
		/// 0:同期しない。即座に送る。
		/// 1:垂直同期を１回待って送る。
		/// 2:垂直同期を２回待って送る。
		/// 　・
		/// 　・
		/// 　・
		/// n:垂直同期をｎ回待って送る。
		/// </param>
		void Present(UINT syncInterval);
		/// <summary>
		/// バックバッファを入れ替える。
		/// </summary>
		/// <returns></returns>
		void SwapBackBuffer();
		/// <summary>
		/// 現在のバックバッファの番号を取得。
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
		enum { FRAME_BUFFER_COUNT = 2 };						//フレームバッファの数。
		ID3D12DescriptorHeap* m_rtvHeap = nullptr;						//レンダリングターゲットビューのディスクリプタヒープ。
		ID3D12DescriptorHeap* m_dsvHeap = nullptr;						//深度ステンシルビューのディスクリプタヒープ。
		UINT m_rtvDescriptorSize = 0;		//フレームバッファのディスクリプタのサイズ。
		UINT m_dsvDescriptorSize = 0;		//深度ステンシルバッファのディスクリプタのサイズ
		ID3D12Resource* m_renderTargets[FRAME_BUFFER_COUNT] = { nullptr };	//レンダリングターゲット。
		ID3D12Resource* m_depthStencilBuffer = nullptr;						//深度ステンシルバッファ。
		D3D12_VIEWPORT m_viewport;			//ビューポート。
		D3D12_RECT m_scissorRect;			//シザリング矩形。
		D3D12_CPU_DESCRIPTOR_HANDLE m_currentFrameBufferRTVHandle;		//レンダリングターゲットビューのディスクリプタハンドル。
		D3D12_CPU_DESCRIPTOR_HANDLE m_currentFrameBufferDSVHandle;		//深度ステンシルビューのディスクリプタハンドル
		UINT m_backBufferIndex = 0;
		IDXGISwapChain3* m_swapChain = nullptr;							//スワップチェイン。
	};
}