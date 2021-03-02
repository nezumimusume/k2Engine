#pragma once

inline void RenderContext::SetDescriptorHeap(DescriptorHeap& descHeap)
{
	m_descriptorHeaps[0] = descHeap.Get();
	m_commandList->SetDescriptorHeaps(1, m_descriptorHeaps);

	//ディスクリプタテーブルに登録する。
	if (descHeap.IsRegistConstantBuffer()) {
		SetGraphicsRootDescriptorTable(0, descHeap.GetConstantBufferGpuDescritorStartHandle());
	}
	if (descHeap.IsRegistShaderResource()) {
		SetGraphicsRootDescriptorTable(1, descHeap.GetShaderResourceGpuDescritorStartHandle());
	}
	if (descHeap.IsRegistUavResource()) {
		SetGraphicsRootDescriptorTable(2, descHeap.GetUavResourceGpuDescritorStartHandle());
	}
}
inline void RenderContext::SetComputeDescriptorHeap(DescriptorHeap& descHeap)
{
	m_descriptorHeaps[0] = descHeap.Get();
	m_commandList->SetDescriptorHeaps(1, m_descriptorHeaps);

	//ディスクリプタテーブルに登録する。
	if (descHeap.IsRegistConstantBuffer()) {
		SetComputeRootDescriptorTable(0, descHeap.GetConstantBufferGpuDescritorStartHandle());
	}
	if (descHeap.IsRegistShaderResource()) {
		SetComputeRootDescriptorTable(1, descHeap.GetShaderResourceGpuDescritorStartHandle());
	}
	if (descHeap.IsRegistUavResource()) {
		SetComputeRootDescriptorTable(2, descHeap.GetUavResourceGpuDescritorStartHandle());
	}
}
inline void RenderContext::WaitUntilFinishDrawingToRenderTarget(RenderTarget& renderTarget)
{
	WaitUntilFinishDrawingToRenderTarget(renderTarget.GetRenderTargetTexture().Get());
}
inline void RenderContext::WaitUntilToPossibleSetRenderTarget(RenderTarget& renderTarget)
{
	WaitUntilToPossibleSetRenderTarget(renderTarget.GetRenderTargetTexture().Get());
}
inline void RenderContext::WaitUntilToPossibleSetRenderTargets(int numRt, RenderTarget* renderTargets[])
{
	for (int i = 0; i < numRt; i++) {
		WaitUntilToPossibleSetRenderTarget(*renderTargets[i]);
	}
}
inline void RenderContext::WaitUntilFinishDrawingToRenderTargets(int numRt, RenderTarget* renderTargets[])
{
	for (int i = 0; i < numRt; i++) {
		WaitUntilFinishDrawingToRenderTarget(*renderTargets[i]);
	}
}
inline void RenderContext::SetRenderTargets(UINT numRT, RenderTarget* renderTargets[])
{
	//d
	D3D12_CPU_DESCRIPTOR_HANDLE rtDSHandleTbl[32];
	int rtNo = 0;
	for( UINT rtNo = 0; rtNo < numRT; rtNo++){
		rtDSHandleTbl[rtNo] = renderTargets[rtNo]->GetRTVCpuDescriptorHandle();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE dsDS = renderTargets[0]->GetDSVCpuDescriptorHandle();
	m_commandList->OMSetRenderTargets(numRT, rtDSHandleTbl, FALSE, &dsDS);

}
inline void RenderContext::ClearRenderTargetViews(int numRt, RenderTarget* renderTargets[])
{
	ClearDepthStencilView(renderTargets[0]->GetDSVCpuDescriptorHandle(), renderTargets[0]->GetDSVClearValue());
	for (int i = 0; i < numRt; i++) {
		ClearRenderTargetView(renderTargets[i]->GetRTVCpuDescriptorHandle(), renderTargets[i]->GetRTVClearColor());
	}
}
inline void RenderContext::SetPipelineState(raytracing::PSO& pso)
{
	m_commandList->SetPipelineState1(pso.Get());
}
