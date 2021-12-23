#include "k2EngineLowPreCompile.h"
#include "FrameBuffer.h"

namespace nsK2EngineLow {
	bool FrameBuffer::Init(
		HWND hwnd,
		ID3D12Device5* d3dDevice,
		ID3D12CommandQueue* commandQueue,
		IDXGIFactory4* dxgiFactory,
		UINT frameBufferWidth,
		UINT frameBufferHeight
	)
	{
		Release();
		//スワップチェインを作成。
		if (!CreateSwapChain(hwnd, frameBufferWidth, frameBufferHeight, dxgiFactory, commandQueue)) {
			//スワップチェインの作成に失敗。
			MessageBox(hwnd, TEXT("スワップチェインの作成に失敗しました。"), TEXT("エラー"), MB_OK);
			return false;
		}

		//フレームバッファ用のディスクリプタヒープを作成する。
		if (!CreateDescriptorHeapForFrameBuffer(d3dDevice)) {
			MessageBox(hwnd, TEXT("フレームバッファ用のディスクリプタヒープの作成に失敗しました。"), TEXT("エラー"), MB_OK);
			return false;
		}

		//フレームバッファ用のRTVを作成する。
		if (!CreateRTVForFameBuffer(d3dDevice)) {
			MessageBox(hwnd, TEXT("フレームバッファ用のRTVの作成に失敗しました。"), TEXT("エラー"), MB_OK);
			return false;

		}

		//フレームバッファ用のDSVを作成する。
		if (!CreateDSVForFrameBuffer(d3dDevice, frameBufferWidth, frameBufferHeight)) {
			MessageBox(hwnd, TEXT("フレームバッファ用のDSVの作成に失敗しました。"), TEXT("エラー"), MB_OK);
			return false;
		}


		//ビューポートを初期化。
		m_viewport.TopLeftX = 0;
		m_viewport.TopLeftY = 0;
		m_viewport.Width = static_cast<FLOAT>(frameBufferWidth);
		m_viewport.Height = static_cast<FLOAT>(frameBufferHeight);
		m_viewport.MinDepth = D3D12_MIN_DEPTH;
		m_viewport.MaxDepth = D3D12_MAX_DEPTH;

		//シザリング矩形を初期化。
		m_scissorRect.left = 0;
		m_scissorRect.top = 0;
		m_scissorRect.right = frameBufferWidth;
		m_scissorRect.bottom = frameBufferHeight;

		m_currentFrameBufferRTVHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

		return true;
	}
	FrameBuffer::~FrameBuffer()
	{
		Release();
	}
	void FrameBuffer::Release()
	{
		if (m_swapChain) {
			m_swapChain->Release();
			m_swapChain = nullptr;
		}

		if (m_rtvHeap) {
			m_rtvHeap->Release();
			m_rtvHeap = nullptr;
		}
		if (m_dsvHeap) {
			m_dsvHeap->Release();
			m_dsvHeap = nullptr;
		}
		for (auto& rt : m_renderTargets) {
			if (rt) {
				rt->Release();
				rt = nullptr;
			}
		}
		if (m_depthStencilBuffer) {
			m_depthStencilBuffer->Release();
			m_depthStencilBuffer = nullptr;
		}
	}
	void FrameBuffer::SwapBackBuffer()
	{
		m_backBufferIndex = 1 ^ m_swapChain->GetCurrentBackBufferIndex();
		m_currentFrameBufferRTVHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
		m_currentFrameBufferRTVHandle.ptr += m_backBufferIndex * m_rtvDescriptorSize;

	}
	bool FrameBuffer::CreateSwapChain(
		HWND hwnd,
		UINT frameBufferWidth,
		UINT frameBufferHeight,
		IDXGIFactory4* dxgiFactory,
		ID3D12CommandQueue* commandQueue
	)
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = FRAME_BUFFER_COUNT;
		swapChainDesc.Width = frameBufferWidth;
		swapChainDesc.Height = frameBufferHeight;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc = {};
		fullScreenDesc.RefreshRate.Denominator = 1;
		fullScreenDesc.RefreshRate.Numerator = 60;
		fullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		fullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		fullScreenDesc.Windowed = true;

		IDXGISwapChain1* swapChain;
		dxgiFactory->CreateSwapChainForHwnd(
			commandQueue,
			hwnd,
			&swapChainDesc,
			&fullScreenDesc,
			nullptr,
			&swapChain
		);
		
		//IDXGISwapChain3のインターフェースを取得。
		swapChain->QueryInterface(IID_PPV_ARGS(&m_swapChain));
		swapChain->Release();

		return true;
	}

	bool FrameBuffer::CreateDescriptorHeapForFrameBuffer(ID3D12Device5* d3dDevice)
	{
		//RTV用のディスクリプタヒープを作成する。
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = FRAME_BUFFER_COUNT;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		auto hr = d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_rtvHeap));
		if (FAILED(hr)) {
			//RTV用のディスクリプタヒープの作成に失敗した。
			return false;
		}
		//ディスクリプタのサイズを取得。
		m_rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		//DSV用のディスクリプタヒープを作成する。
		desc.NumDescriptors = 1;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		hr = d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_dsvHeap));
		if (FAILED(hr)) {
			//DSV用のディスクリプタヒープの作成に失敗した。
			return false;
		}
		//ディスクリプタのサイズを取得。
		m_dsvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		return true;
	}
	bool FrameBuffer::CreateRTVForFameBuffer(ID3D12Device5* d3dDevice)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

		//フロントバッファをバックバッファ用のRTVを作成。
		for (UINT n = 0; n < FRAME_BUFFER_COUNT; n++) {
			m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n]));
			d3dDevice->CreateRenderTargetView(
				m_renderTargets[n], nullptr, rtvHandle
			);
			m_renderTargets[n]->SetName(L"FrameBuffer::RenderTargetView");
			
			rtvHandle.ptr += m_rtvDescriptorSize;
		}
		return true;
	}
	bool FrameBuffer::CreateDSVForFrameBuffer(ID3D12Device5* d3dDevice, UINT frameBufferWidth, UINT frameBufferHeight)
	{
		D3D12_CLEAR_VALUE dsvClearValue;
		dsvClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		dsvClearValue.DepthStencil.Depth = 1.0f;
		dsvClearValue.DepthStencil.Stencil = 0;

		CD3DX12_RESOURCE_DESC desc(
			D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			0,
			frameBufferWidth,
			frameBufferHeight,
			1,
			1,
			DXGI_FORMAT_D32_FLOAT,
			1,
			0,
			D3D12_TEXTURE_LAYOUT_UNKNOWN,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto hr = d3dDevice->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&dsvClearValue,
			IID_PPV_ARGS(&m_depthStencilBuffer)
		);
		if (FAILED(hr)) {
			//深度ステンシルバッファの作成に失敗。
			return false;
		}
		//ディスクリプタを作成
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();

		d3dDevice->CreateDepthStencilView(
			m_depthStencilBuffer, nullptr, dsvHandle
		);

		//深度ステンシルバッファのディスクリプタヒープの開始アドレスを取得。
		m_currentFrameBufferDSVHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();

		return true;
	}

	void FrameBuffer::Present(UINT syncInterval)
	{
		m_swapChain->Present(syncInterval, 0);
	}
}