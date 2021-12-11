#include "k2EngineLowPreCompile.h"
#include "PipelineState.h"

namespace nsK2EngineLow {
	PipelineState::~PipelineState()
	{
		Release();
	}
	void PipelineState::Release()
	{
		ReleaseD3D12Object(m_pipelineState);
	}
	void PipelineState::Init(D3D12_GRAPHICS_PIPELINE_STATE_DESC desc)
	{
		Release();
		auto d3dDevice = g_graphicsEngine->GetD3DDevice();
		auto hr = d3dDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&m_pipelineState));
		if (FAILED(hr)) {
			MessageBoxA(nullptr, "パイプラインステートの作成に失敗しました。\n", "エラー", MB_OK);
			std::abort();
		}
	}
	void PipelineState::Init(D3D12_COMPUTE_PIPELINE_STATE_DESC desc)
	{
		Release();
		auto d3dDevice = g_graphicsEngine->GetD3DDevice();
		auto hr = d3dDevice->CreateComputePipelineState(&desc, IID_PPV_ARGS(&m_pipelineState));
		if (FAILED(hr)) {
			MessageBoxA(nullptr, "パイプラインステートの作成に失敗しました。\n", "エラー", MB_OK);
			std::abort();
		}
	}
}
