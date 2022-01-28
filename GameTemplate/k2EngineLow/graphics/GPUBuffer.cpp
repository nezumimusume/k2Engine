#include "k2EngineLowPreCompile.h"
#include "GPUBuffer.h"

namespace nsK2EngineLow {
	GPUBuffer::~GPUBuffer()
	{
		Release();
	}
	void GPUBuffer::Release()
	{
		ReleaseD3D12Object(m_buffer);
	}
	void GPUBuffer::Init(D3D12_RESOURCE_DESC desc)
	{
		Release();
		m_desc = desc;
		//todo ‚Ü‚Æ‚ß‚½•û‚ª‚¦‚¦‚ÈB
		const D3D12_HEAP_PROPERTIES kDefaultHeapProps =
		{
			D3D12_HEAP_TYPE_DEFAULT,
			D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
			D3D12_MEMORY_POOL_UNKNOWN,
			0,
			0
		};

		auto d3dDevice = g_graphicsEngine->GetD3DDevice();
		d3dDevice->CreateCommittedResource(
			&kDefaultHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COPY_SOURCE,
			nullptr,
			IID_PPV_ARGS(&m_buffer)
		);
	}
	void GPUBuffer::RegistUnorderAccessView(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, int bufferNo)
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		if (m_desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D) {
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		}
		else {
			//–¢‘Î‰žB
			std::abort();
		}
		auto d3dDevice = g_graphicsEngine->GetD3DDevice();
		d3dDevice->CreateUnorderedAccessView(
			m_buffer,
			nullptr,
			&uavDesc,
			cpuHandle
		);


	}
	void GPUBuffer::RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo)
	{
		auto device = g_graphicsEngine->GetD3DDevice();
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = m_desc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = m_desc.MipLevels;
		device->CreateShaderResourceView(m_buffer, &srvDesc, descriptorHandle);
	}
}