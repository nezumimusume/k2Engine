#include "k2EngineLowPreCompile.h"
#include "GraphicsEngine.h"
#include <pix.h>

namespace nsK2EngineLow {
	GraphicsEngine* g_graphicsEngine = nullptr;	//�O���t�B�b�N�X�G���W��
	Camera* g_camera2D = nullptr;				//2D�J�����B
	Camera* g_camera3D = nullptr;				//3D�J�����B
	/// <summary>
	/// DXR���T�|�[�g����Ă��邩�𒲂ׂ�B
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
		//��n���B
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

		//�`��I���҂�
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

		//�f�o�C�X�ɃA�N�Z�X���邽�߂̃C���^�[�t�F�[�X���쐬�B
		auto dxgiFactory = CreateDXGIFactory();

		//D3D�f�o�C�X�̍쐬�B
		if (!CreateD3DDevice(dxgiFactory)) {
			//D3D�f�o�C�X�̍쐬�Ɏ��s�����B
			MessageBox(hwnd, TEXT("D3D�f�o�C�X�̍쐬�Ɏ��s���܂����B"), TEXT("�G���["), MB_OK);
			return false;
		}
		// DXR�̃T�|�[�g�𒲂ׂ�B
		if (IsSupportDXR(m_d3dDevice)) {
			// DXR���T�|�[�g����Ă��āA����RTX�V���[�Y�Ȃ烌�C�g���\�ɂ���B
			m_isPossibleRaytracing = m_isPossibleRaytracing && true;
		}
		//�R�}���h�L���[�̍쐬�B
		if (!CreateCommandQueue()) {
			//�R�}���h�L���[�̍쐬�Ɏ��s�����B
			MessageBox(hwnd, TEXT("�R�}���h�L���[�̍쐬�Ɏ��s���܂����B"), TEXT("�G���["), MB_OK);
			return false;
		}

		//�t���[���o�b�t�@��������
		if (!m_frameBuffer.Init(hwnd, m_d3dDevice, m_commandQueue, dxgiFactory, frameBufferWidth, frameBufferHeight)) {
			MessageBox(hwnd, TEXT("�t���[���o�b�t�@�̍쐬�Ɏ��s���܂����B"), TEXT("�G���["), MB_OK);
			return false;
		}

		//�R�}���h�A���P�[�^�̍쐬�B
		for (auto& commandAllocator : m_commandAllocator) {
			m_d3dDevice->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(&commandAllocator));

			if (!commandAllocator) {
				MessageBox(hwnd, TEXT("�R�}���h�A���P�[�^�̍쐬�Ɏ��s���܂����B"), TEXT("�G���["), MB_OK);
				return false;
			}
		}

		//�R�}���h���X�g�̍쐬�B
		if (!CreateCommandList()) {
			MessageBox(hwnd, TEXT("�R�}���h���X�g�̍쐬�Ɏ��s���܂����B"), TEXT("�G���["), MB_OK);
			return false;
		}

		//GPU�Ɠ������Ƃ邽�߂̃I�u�W�F�N�g���쐬����B
		if (!CreateSynchronizationWithGPUObject()) {
			MessageBox(hwnd, TEXT("GPU�Ɠ������Ƃ邽�߂̃I�u�W�F�N�g�̍쐬�Ɏ��s���܂����B"), TEXT("�G���["), MB_OK);
			return false;
		}

		//�����_�����O�R���e�L�X�g�̍쐬�B
		m_renderContext.Init(m_commandList[m_frameIndex]);


		//CBR_SVR�̃f�B�X�N���v�^�̃T�C�Y���擾�B
		m_cbrSrvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		//Sampler�̃f�B�X�N���v�^�̃T�C�Y���擾�B
		m_samplerDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

		//���������I������̂�DXGI��j���B
		dxgiFactory->Release();

		//�k���e�N�X�`����������
		m_nullTextureMaps.Init();

		//�J����������������B
		m_camera2D.SetUpdateProjMatrixFunc(Camera::enUpdateProjMatrixFunc_Ortho);
		m_camera2D.SetWidth(static_cast<float>(m_frameBufferWidth));
		m_camera2D.SetHeight(static_cast<float>(m_frameBufferHeight));
		m_camera2D.SetPosition({ 0.0f, 0.0f, -1.0f });
		m_camera2D.SetTarget({ 0.0f, 0.0f, 0.0f });

		m_camera3D.SetPosition({ 0.0f, 50.0f, -200.0f });
		m_camera3D.SetTarget({ 0.0f, 50.0f, 0.0f });

		g_camera2D = &m_camera2D;
		g_camera3D = &m_camera3D;

		//DirectXTK�p�̃O���t�B�bs�N�������Ǘ��N���X�̃C���X�^���X���쐬����B
		m_directXTKGfxMemroy = std::make_unique<DirectX::GraphicsMemory>(m_d3dDevice);
		//�t�H���g�`��G���W�����������B
		m_fontEngine.Init();
		//���C�g���G���W�����������B
		m_raytracingEngine.Init(raytracingInitData);
		return true;
	}

	IDXGIFactory4* GraphicsEngine::CreateDXGIFactory()
	{
		UINT dxgiFactoryFlags = 0;
#ifdef K2_DEBUG
		//�f�o�b�O�R���g���[���[������΁A�f�o�b�O���C���[������DXGI���쐬����B
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
				D3D_FEATURE_LEVEL_12_1,	//Direct3D 12.1�̋@�\���g���B
				D3D_FEATURE_LEVEL_12_0	//Direct3D 12.0�̋@�\���g���B
		};
		IDXGIAdapter* adapterTmp = nullptr;
		IDXGIAdapter* adapterVender[Num_GPUVender] = { nullptr };	//�e�x���_�[�̃A�_�v�^�[�B
		IDXGIAdapter* adapterMaxVideoMemory = nullptr;				//�ő�r�f�I�������̃A�_�v�^�B
		IDXGIAdapter* useAdapter = nullptr;							//�ŏI�I�Ɏg�p����A�_�v�^�B
		SIZE_T videoMemorySize = 0;
		for (int i = 0; dxgiFactory->EnumAdapters(i, &adapterTmp) != DXGI_ERROR_NOT_FOUND; i++) {
			DXGI_ADAPTER_DESC desc;
			adapterTmp->GetDesc(&desc);
			if (desc.DedicatedVideoMemory > videoMemorySize) {
				//������̃r�f�I�������̕��������̂ŁA��������g���B
				if (adapterMaxVideoMemory != nullptr) {
					adapterMaxVideoMemory->Release();
				}
				adapterMaxVideoMemory = adapterTmp;
				adapterMaxVideoMemory->AddRef();
				videoMemorySize = desc.DedicatedVideoMemory;
			}
			if (wcsstr(desc.Description, L"NVIDIA") != nullptr) {
				//NVIDIA��
#ifdef ENABLE_DXR_ON_RTX_ONLY
				std::wstring adapterName =desc.Description;
				if (adapterName.find(L"RTX") != std::string::npos) {
					// RTX�V���[�Y��GPU�B
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
				//AMD��
				if (adapterVender[GPU_VenderAMD]) {
					adapterVender[GPU_VenderAMD]->Release();
				}
				adapterVender[GPU_VenderAMD] = adapterTmp;
				adapterVender[GPU_VenderAMD]->AddRef();
			}
			else if (wcsstr(desc.Description, L"Intel") != nullptr) {
				//Intel��
				if (adapterVender[GPU_VenderIntel]) {
					adapterVender[GPU_VenderIntel]->Release();
				}
				adapterVender[GPU_VenderIntel] = adapterTmp;
				adapterVender[GPU_VenderIntel]->AddRef();
			}
			adapterTmp->Release();
		}
		//�g�p����A�_�v�^�[�����߂�B
		if (adapterVender[GPU_VenderNvidia] != nullptr) {
			//NVIDIA�����ŗD��
			useAdapter = adapterVender[GPU_VenderNvidia];
		}
		else if (adapterVender[GPU_VenderAMD] != nullptr) {
			//����AMD���D��B
			useAdapter = adapterVender[GPU_VenderAMD];
		}
		else {
			//NVIDIA��AMD��GPU���Ȃ���΃r�f�I����������ԑ�������g���B
			useAdapter = adapterMaxVideoMemory;
		}
		for (auto featureLevel : featureLevels) {
			auto hr = D3D12CreateDevice(
				useAdapter,
				featureLevel,
				IID_PPV_ARGS(&m_d3dDevice)
			);
			if (SUCCEEDED(hr)) {
				//D3D�f�o�C�X�̍쐬�ɐ��������B
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
		// Windows11�̕s��̉��Ή�
		// Windows11��MISMATCHING_COMMAND_LIST_TYPE���o��悤�ɂȂ��Ă���A
		// WindowsSDK�̕s��Ƃ̂��ƁB
		// �ߓ����ɏC�������炵���B
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
			//�R�}���h�L���[�̍쐬�Ɏ��s�����B
			return false;
		}

		return true;
	}

	bool GraphicsEngine::CreateCommandList()
	{
		int listNo = 0;
		for (auto& commandList : m_commandList) {
			//�R�}���h���X�g�̍쐬�B
			m_d3dDevice->CreateCommandList(
				0, D3D12_COMMAND_LIST_TYPE_DIRECT,
				m_commandAllocator[listNo],
				nullptr, IID_PPV_ARGS(&commandList)
			);
			if (!commandList) {
				return false;
			}
			//�R�}���h���X�g�͊J����Ă����Ԃō쐬�����̂ŁA�����������B
			commandList->Close();
			
			listNo++;
		}
		return true;
	}
	bool GraphicsEngine::CreateSynchronizationWithGPUObject()
	{
		m_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
		if (!m_fence) {
			//�t�F���X�̍쐬�Ɏ��s�����B
			return false;
		}
		m_fenceValue = 1;
		//�������s���Ƃ��̃C�x���g�n���h�����쐬����B
		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_fenceEvent == nullptr) {
			return false;
		}
		return true;
	}
	void GraphicsEngine::BeginRender()
	{
		m_frameIndex = m_frameBuffer.GetCurrentBackBufferIndex();

		//�J�������X�V����B
		m_camera2D.Update();
		m_camera3D.Update();

		//�R�}���h�A���P�[�^�������Z�b�g�B
		m_commandAllocator[m_frameIndex]->Reset();

		m_renderContext.SetCommandList(m_commandList[m_frameIndex]);
		//�����_�����O�R���e�L�X�g�����Z�b�g�B
		m_renderContext.Reset(m_commandAllocator[m_frameIndex], m_pipelineState);


		//�t���[���o�b�t�@�������_�����O�^�[�Q�b�g�Ƃ��Đݒ�\�ɂȂ�܂ő҂B
		m_renderContext.WaitUntilToPossibleSetRenderTarget(m_frameBuffer.GetCurrentRenderTarget());

		//�t���[���o�b�t�@�������_�����O�^�[�Q�b�g��ݒ�B
		m_renderContext.SetRenderTarget(
			m_frameBuffer.GetCurrentRenderTargetViewDescriptorHandle(),
			m_frameBuffer.GetCurrentDepthStencilViewDescriptorHandle()
		);
		m_renderContext.SetViewportAndScissor(m_frameBuffer.GetViewport());
		//�t���[���o�b�t�@���N���A�B
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
				// �J��
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
		// �����_�����O�^�[�Q�b�g�ւ̕`�����݊����҂�
		m_renderContext.WaitUntilFinishDrawingToRenderTarget(m_frameBuffer.GetCurrentRenderTarget());
			
		if (m_isExecuteCommandList)
		{
#ifdef USE_FPS_LIMITTER
			m_frameBuffer.Present(1);
#else
			m_frameBuffer.Present(0);
#endif
			// �`�抮���҂��B
			WaitDraw();
		}

		m_directXTKGfxMemroy->Commit(m_commandQueue);
		//�����_�����O�R���e�L�X�g�����B
		m_renderContext.Close();
		//���̃t���[���ɍ쐬�����`��R�}���h�����s����B
		ID3D12CommandList* ppCommandLists[] = { m_commandList[m_frameIndex] };
		m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		// �R�}���h���X�g��GPU�ɗ�������B
		m_isExecuteCommandList = true;
		m_directXTKGfxMemroy->GarbageCollect();

		// �o�b�N�o�b�t�@�����ւ���B
		m_frameBuffer.SwapBackBuffer();

		// D3D12�I�u�W�F�N�g�̉�����N�G�X�g����������B
		ExecuteRequestReleaseD3D12Object();

	}
}
