#pragma once

namespace nsK2EngineLow {
	inline ID3D12DescriptorHeap* DescriptorHeap::Get()const
	{
		return m_descriptorHeap;
	}
	inline D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetConstantBufferGpuDescritorStartHandle() const
	{
		auto backBufferIndex = GetBackBufferNo();
		return m_cbGpuDescriptorStart[backBufferIndex];
	}
	inline D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetShaderResourceGpuDescritorStartHandle() const
	{
		auto backBufferIndex = GetBackBufferNo();
		return m_srGpuDescriptorStart[backBufferIndex];
	}
	inline D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetUavResourceGpuDescritorStartHandle() const
	{
		auto backBufferIndex = GetBackBufferNo();
		return m_uavGpuDescriptorStart[backBufferIndex];
	}
	inline D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetSamplerResourceGpuDescritorStartHandle() const
	{
		auto backBufferIndex = GetBackBufferNo();
		return m_samplerGpuDescriptorStart[backBufferIndex];
	}
}