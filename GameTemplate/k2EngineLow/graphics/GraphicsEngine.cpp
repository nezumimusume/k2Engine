#include "k2EngineLowPreCompile.h"
#include "GraphicsEngine.h"
#include <pix.h>

namespace nsK2EngineLow {
	GraphicsEngine* g_graphicsEngine = nullptr;	//グラフィックスエンジン
	Camera* g_camera2D = nullptr;				//2Dカメラ。
	Camera* g_camera3D = nullptr;				//3Dカメラ。
	/// <summary>
	/// DXRがサポートされているかを調べる。
	/// </summary>
	/// <param name="pDevice"></param>
	/// <returns></returns>
	bool IsSupportDXR(ID3D12Device5* pDevice)
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS5 options = {};
		auto hr = pDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options, sizeof(options));
		if (FAILED(hr))
		{
			return false;
		}

		return options.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
	}
	GraphicsEngine::~GraphicsEngine()
	{
		WaitDraw();

		for (auto& req : m_reqDelayRelease3d12ObjectList) {
			if (req.d3dObject) {
				req.d3dObject->Release();
			}
		}
		//後始末。
		if (m_commandQueue) {
			m_commandQueue->Release();
		}


		m_frameBuffer.Release();

		for (auto& commandAllocator : m_commandAllocator) {
			if (commandAllocator) {
				commandAllocator->Release();
			}
		}
		for (auto& commandList : m_commandList) {
			if (commandList) {
				commandList->Release();
			}
		}
		if (m_pipelineState) {
			m_pipelineState->Release();
		}

		if (m_fence) {
			m_fence->Release();
		}

		if (m_d3dDevice) {
			m_d3dDevice->Release();
		}
		
		CloseHandle(m_fenceEvent);
	}
	void GraphicsEngine::WaitDraw()
	{

		//描画終了待ち
		// Signal and increment the fence value.
		const UINT64 fence = m_fenceValue;
		m_commandQueue->Signal(m_fence, fence);
		m_fenceValue++;

		// Wait until the previous frame is finished.
		if (m_fence->GetCompletedValue() < fence)
		{
			m_fence->SetEventOnCompletion(fence, m_fenceEvent);
			WaitForSingleObject(m_fenceEvent, INFINITE);
		}
	}
	bool GraphicsEngine::Init(
		HWND hwnd, 
		UINT frameBufferWidth, 
		UINT frameBufferHeight,
		const raytracing::InitData& raytracingInitData
	)
	{
		//
		g_graphicsEngine = this;

		m_frameBufferWidth = frameBufferWidth;
		m_frameBufferHeight = frameBufferHeight;

		//デバイスにアクセスするためのインターフェースを作成。
		auto dxgiFactory = CreateDXGIFactory();

		//D3Dデバイスの作成。
		if (!CreateD3DDevice(dxgiFactory)) {
			//D3Dデバイスの作成に失敗した。
			MessageBox(hwnd, TEXT("D3Dデバイスの作成に失敗しました。"), TEXT("エラー"), MB_OK);
			return false;
		}
		// DXRのサポートを調べる。
		if (IsSupportDXR(m_d3dDevice)) {
			// DXRがサポートされていて、かつRTXシリーズならレイトレ可能にする。
			m_isPossibleRaytracing = m_isPossibleRaytracing && true;
		}
		//コマンドキューの作成。
		if (!CreateCommandQueue()) {
			//コマンドキューの作成に失敗した。
			MessageBox(hwnd, TEXT("コマンドキューの作成に失敗しました。"), TEXT("エラー"), MB_OK);
			return false;
		}

		//フレームバッファを初期化
		if (!m_frameBuffer.Init(hwnd, m_d3dDevice, m_commandQueue, dxgiFactory, frameBufferWidth, frameBufferHeight)) {
			MessageBox(hwnd, TEXT("フレームバッファの作成に失敗しました。"), TEXT("エラー"), MB_OK);
			return false;
		}

		//コマンドアロケータの作成。
		for (auto& commandAllocator : m_commandAllocator) {
			m_d3dDevice->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(&commandAllocator));

			if (!commandAllocator) {
				MessageBox(hwnd, TEXT("コマンドアロケータの作成に失敗しました。"), TEXT("エラー"), MB_OK);
				return false;
			}
		}

		//コマンドリストの作成。
		if (!CreateCommandList()) {
			MessageBox(hwnd, TEXT("コマンドリストの作成に失敗しました。"), TEXT("エラー"), MB_OK);
			return false;
		}

		//GPUと同期をとるためのオブジェクトを作成する。
		if (!CreateSynchronizationWithGPUObject()) {
			MessageBox(hwnd, TEXT("GPUと同期をとるためのオブジェクトの作成に失敗しました。"), TEXT("エラー"), MB_OK);
			return false;
		}

		//レンダリングコンテキストの作成。
		m_renderContext.Init(m_commandList[m_frameIndex]);


		//CBR_SVRのディスクリプタのサイズを取得。
		m_cbrSrvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		//Samplerのディスクリプタのサイズを取得。
		m_samplerDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

		//初期化が終わったのでDXGIを破棄。
		dxgiFactory->Release();

		//ヌルテクスチャを初期化
		m_nullTextureMaps.Init();

		//カメラを初期化する。
		m_camera2D.SetUpdateProjMatrixFunc(Camera::enUpdateProjMatrixFunc_Ortho);
		m_camera2D.SetWidth(static_cast<float>(m_frameBufferWidth));
		m_camera2D.SetHeight(static_cast<float>(m_frameBufferHeight));
		m_camera2D.SetPosition({ 0.0f, 0.0f, -1.0f });
		m_camera2D.SetTarget({ 0.0f, 0.0f, 0.0f });

		m_camera3D.SetPosition({ 0.0f, 50.0f, -200.0f });
		m_camera3D.SetTarget({ 0.0f, 50.0f, 0.0f });

		g_camera2D = &m_camera2D;
		g_camera3D = &m_camera3D;

		//DirectXTK用のグラフィッsクメモリ管理クラスのインスタンスを作成する。
		m_directXTKGfxMemroy = std::make_unique<DirectX::GraphicsMemory>(m_d3dDevice);
		//フォント描画エンジンを初期化。
		m_fontEngine.Init();
		//レイトレエンジンを初期化。
		m_raytracingEngine.Init(raytracingInitData);
		return true;
	}

	IDXGIFactory4* GraphicsEngine::CreateDXGIFactory()
	{
		UINT dxgiFactoryFlags = 0;
#ifdef K2_DEBUG
		//デバッグコントローラーがあれば、デバッグレイヤーがあるDXGIを作成する。
		ID3D12Debug* debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();

			// Enable additional debug layers.
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			debugController->Release();
		}
#endif
		IDXGIFactory4* factory;
		CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));
		return factory;
	}

	bool GraphicsEngine::CreateD3DDevice(IDXGIFactory4* dxgiFactory)
	{
		D3D_FEATURE_LEVEL featureLevels[] = {
				D3D_FEATURE_LEVEL_12_1,	//Direct3D 12.1の機能を使う。
				D3D_FEATURE_LEVEL_12_0	//Direct3D 12.0の機能を使う。
		};
		IDXGIAdapter* adapterTmp = nullptr;
		IDXGIAdapter* adapterVender[Num_GPUVender] = { nullptr };	//各ベンダーのアダプター。
		IDXGIAdapter* adapterMaxVideoMemory = nullptr;				//最大ビデオメモリのアダプタ。
		IDXGIAdapter* useAdapter = nullptr;							//最終的に使用するアダプタ。
		SIZE_T videoMemorySize = 0;
		for (int i = 0; dxgiFactory->EnumAdapters(i, &adapterTmp) != DXGI_ERROR_NOT_FOUND; i++) {
			DXGI_ADAPTER_DESC desc;
			adapterTmp->GetDesc(&desc);
			if (desc.DedicatedVideoMemory > videoMemorySize) {
				//こちらのビデオメモリの方が多いので、こちらを使う。
				if (adapterMaxVideoMemory != nullptr) {
					adapterMaxVideoMemory->Release();
				}
				adapterMaxVideoMemory = adapterTmp;
				adapterMaxVideoMemory->AddRef();
				videoMemorySize = desc.DedicatedVideoMemory;
			}
			if (wcsstr(desc.Description, L"NVIDIA") != nullptr) {
				//NVIDIA製
#ifdef ENABLE_DXR_ON_RTX_ONLY
				std::wstring adapterName =desc.Description;
				if (adapterName.find(L"RTX") != std::string::npos) {
					// RTXシリーズのGPU。
					m_isPossibleRaytracing = true;

				}
#else // #ifdef ENABLE_DXR_ON_RTX_ONLY
				m_isPossibleRaytracing = true;
#endif // #ifdef ENABLE_DXR_ON_RTX_ONLY
				if (adapterVender[GPU_VenderNvidia]) {
					adapterVender[GPU_VenderNvidia]->Release();
				}
				adapterVender[GPU_VenderNvidia] = adapterTmp;
				adapterVender[GPU_VenderNvidia]->AddRef();
			}
			else if (wcsstr(desc.Description, L"AMD") != nullptr) {
				//AMD製
				if (adapterVender[GPU_VenderAMD]) {
					adapterVender[GPU_VenderAMD]->Release();
				}
				adapterVender[GPU_VenderAMD] = adapterTmp;
				adapterVender[GPU_VenderAMD]->AddRef();
			}
			else if (wcsstr(desc.Description, L"Intel") != nullptr) {
				//Intel製
				if (adapterVender[GPU_VenderIntel]) {
					adapterVender[GPU_VenderIntel]->Release();
				}
				adapterVender[GPU_VenderIntel] = adapterTmp;
				adapterVender[GPU_VenderIntel]->AddRef();
			}
			adapterTmp->Release();
		}
		//使用するアダプターを決める。
		if (adapterVender[GPU_VenderNvidia] != nullptr) {
			//NVIDIA製が最優先
			useAdapter = adapterVender[GPU_VenderNvidia];
		}
		else if (adapterVender[GPU_VenderAMD] != nullptr) {
			//次はAMDが優先。
			useAdapter = adapterVender[GPU_VenderAMD];
		}
		else {
			//NVIDIAとAMDのGPUがなければビデオメモリが一番多いやつを使う。
			useAdapter = adapterMaxVideoMemory;
		}
		for (auto featureLevel : featureLevels) {
			auto hr = D3D12CreateDevice(
				useAdapter,
				featureLevel,
				IID_PPV_ARGS(&m_d3dDevice)
			);
			if (SUCCEEDED(hr)) {
				//D3Dデバイスの作成に成功した。
				break;
			}
		}
		for (auto& adapter : adapterVender) {
			if (adapter) {
				adapter->Release();
			}
		}
		if (adapterMaxVideoMemory) {
			adapterMaxVideoMemory->Release();
		}
#ifdef K2_DEBUG
		// Windows11の不具合の回避対応
		// Windows11でMISMATCHING_COMMAND_LIST_TYPEが出るようになっており、
		// WindowsSDKの不具合とのこと。
		// 近日中に修正されるらしい。
		// https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
		ID3D12InfoQueue* infoQueue;
		if ( m_d3dDevice->QueryInterface(IID_PPV_ARGS(&infoQueue)) == S_OK ) {
			D3D12_MESSAGE_ID hide[] =
			{
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
				// Workarounds for debug layer issues on hybrid-graphics systems
				D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
				D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
			};
			D3D12_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
			filter.DenyList.pIDList = hide;
			infoQueue->AddStorageFilterEntries(&filter);
			infoQueue->Release();
		}
#endif
		return m_d3dDevice != nullptr;
	}
	bool GraphicsEngine::CreateCommandQueue()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		auto hr = m_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));
		if (FAILED(hr)) {
			//コマンドキューの作成に失敗した。
			return false;
		}

		return true;
	}

	bool GraphicsEngine::CreateCommandList()
	{
		int listNo = 0;
		for (auto& commandList : m_commandList) {
			//コマンドリストの作成。
			m_d3dDevice->CreateCommandList(
				0, D3D12_COMMAND_LIST_TYPE_DIRECT,
				m_commandAllocator[listNo],
				nullptr, IID_PPV_ARGS(&commandList)
			);
			if (!commandList) {
				return false;
			}
			//コマンドリストは開かれている状態で作成されるので、いったん閉じる。
			commandList->Close();
			
			listNo++;
		}
		return true;
	}
	bool GraphicsEngine::CreateSynchronizationWithGPUObject()
	{
		m_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
		if (!m_fence) {
			//フェンスの作成に失敗した。
			return false;
		}
		m_fenceValue = 1;
		//同期を行うときのイベントハンドラを作成する。
		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_fenceEvent == nullptr) {
			return false;
		}
		return true;
	}
	void GraphicsEngine::BeginRender()
	{
		m_frameIndex = m_frameBuffer.GetCurrentBackBufferIndex();

		//カメラを更新する。
		m_camera2D.Update();
		m_camera3D.Update();

		//コマンドアロケータををリセット。
		m_commandAllocator[m_frameIndex]->Reset();

		m_renderContext.SetCommandList(m_commandList[m_frameIndex]);
		//レンダリングコンテキストもリセット。
		m_renderContext.Reset(m_commandAllocator[m_frameIndex], m_pipelineState);


		//フレームバッファをレンダリングターゲットとして設定可能になるまで待つ。
		m_renderContext.WaitUntilToPossibleSetRenderTarget(m_frameBuffer.GetCurrentRenderTarget());

		//フレームバッファをレンダリングターゲットを設定。
		m_renderContext.SetRenderTarget(
			m_frameBuffer.GetCurrentRenderTargetViewDescriptorHandle(),
			m_frameBuffer.GetCurrentDepthStencilViewDescriptorHandle()
		);
		m_renderContext.SetViewportAndScissor(m_frameBuffer.GetViewport());
		//フレームバッファをクリア。
		const float clearColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
		m_renderContext.ClearRenderTargetView(m_frameBuffer.GetCurrentRenderTargetViewDescriptorHandle(), clearColor);
		m_renderContext.ClearDepthStencilView(m_frameBuffer.GetCurrentDepthStencilViewDescriptorHandle(), 1.0f);

	}
	void GraphicsEngine::ChangeRenderTargetToFrameBuffer(RenderContext& rc)
	{
		rc.SetRenderTarget(
			m_frameBuffer.GetCurrentRenderTargetViewDescriptorHandle(),
			m_frameBuffer.GetCurrentDepthStencilViewDescriptorHandle()
		);
	}
	void GraphicsEngine::ExecuteRequestReleaseD3D12Object()
	{
		auto releaseReqIt = m_reqDelayRelease3d12ObjectList.begin();
		while (releaseReqIt != m_reqDelayRelease3d12ObjectList.end()) {
			if (releaseReqIt->delayTime == 0) {
				// 開放
				if (releaseReqIt->d3dObject) {
					releaseReqIt->d3dObject->Release();
				}
				releaseReqIt = m_reqDelayRelease3d12ObjectList.erase(releaseReqIt);
			}
			else {
				releaseReqIt->delayTime--;
				releaseReqIt++;
			}
		}
	}
	void GraphicsEngine::EndRender()
	{
		// レンダリングターゲットへの描き込み完了待ち
		m_renderContext.WaitUntilFinishDrawingToRenderTarget(m_frameBuffer.GetCurrentRenderTarget());
			
		if (m_isExecuteCommandList)
		{
#ifdef USE_FPS_LIMITTER
			m_frameBuffer.Present(1);
#else
			m_frameBuffer.Present(0);
#endif
			// 描画完了待ち。
			WaitDraw();
		}

		m_directXTKGfxMemroy->Commit(m_commandQueue);
		//レンダリングコンテキストを閉じる。
		m_renderContext.Close();
		//このフレームに作成した描画コマンドを実行する。
		ID3D12CommandList* ppCommandLists[] = { m_commandList[m_frameIndex] };
		m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		// コマンドリストをGPUに流した印。
		m_isExecuteCommandList = true;
		m_directXTKGfxMemroy->GarbageCollect();

		// バックバッファを入れ替える。
		m_frameBuffer.SwapBackBuffer();

		// D3D12オブジェクトの解放リクエストを処理する。
		ExecuteRequestReleaseD3D12Object();

	}
}
