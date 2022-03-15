#include "k2EngineLowPreCompile.h"

#include "RaytracingEngine.h"
#include "graphics/Material.h"

namespace nsK2EngineLow {
	namespace raytracing {
		using namespace std;

		ID3D12Resource* CreateBuffer(
			ID3D12Device5* pDevice,
			uint64_t size,
			D3D12_RESOURCE_FLAGS flags,
			D3D12_RESOURCE_STATES initState,
			const D3D12_HEAP_PROPERTIES& heapProps)
		{
			D3D12_RESOURCE_DESC bufDesc = {};
			bufDesc.Alignment = 0;
			bufDesc.DepthOrArraySize = 1;
			bufDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			bufDesc.Flags = flags;
			bufDesc.Format = DXGI_FORMAT_UNKNOWN;
			bufDesc.Height = 1;
			bufDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			bufDesc.MipLevels = 1;
			bufDesc.SampleDesc.Count = 1;
			bufDesc.SampleDesc.Quality = 0;
			bufDesc.Width = size;

			ID3D12Resource* pBuffer;
			pDevice->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufDesc, initState, nullptr, IID_PPV_ARGS(&pBuffer));
			return pBuffer;
		}
		void Engine::Init(const InitData& initData)
		{
			auto d3dDevice = g_graphicsEngine->GetD3DDevice();

			// ���C�g���̌��ʂ̏o�͐�̃e�N�X�`�����쐬�B
			D3D12_RESOURCE_DESC resDesc = {};
			resDesc.DepthOrArraySize = 1;
			resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			resDesc.Format = initData.m_outputColorBufferFormat;
			resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			resDesc.Height = g_graphicsEngine->GetFrameBufferHeight();
			resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			resDesc.MipLevels = 1;
			resDesc.SampleDesc.Count = 1;
			resDesc.Width = g_graphicsEngine->GetFrameBufferWidth();
			m_outputResource.Init(resDesc);
			m_outputTexture.InitFromD3DResource(m_outputResource.Get());

			//���C�W�F�l���[�V�����p�̒萔�o�b�t�@�B
			Camera cam;
			cam.pos = g_camera3D->GetPosition();
			cam.mViewProjInv = g_camera3D->GetViewProjectionMatrixInv();
			cam.aspect = g_camera3D->GetAspect();
			cam.fNear = g_camera3D->GetNear();
			cam.fFar = g_camera3D->GetFar();
			// ���C�g���G���W�����Ń_�u���o�b�t�@�ɂ��Ă���̂ŁA�����ł̓_�u���o�b�t�@�ɂ��Ȃ��B
			m_rayGenerationCB[0].Init(sizeof(Camera), &cam, false);
			m_rayGenerationCB[1].Init(sizeof(Camera), &cam, false);

			for (int bufferNo = 0; bufferNo < 2; bufferNo++) {
				m_expandSRV[bufferNo] = std::make_unique< ExpanadSRV>();
				m_expandSRV[bufferNo]->Init(
					initData.m_expandShaderResource,
					initData.m_expandShaderResourceSize
				);
			}
			
		}
		void Engine::CommitRegistGeometry(RenderContext& rc)
		{
			if (!m_isDirty) {
				return;
			}

			m_world.CommitRegistGeometry(rc);
			
			for (int i = 0; i < 2; i++) {
				// �e�탊�\�[�X���f�B�X�N���v�^�q�[�v�ɓo�^����B
				m_descriptorHeaps[i].Init(
					i, 
					m_world, 
					m_outputResource, 
					m_rayGenerationCB[i], 
					m_skycubeBox,
					m_expandSRV[i]->m_structuredBuffer
				);
				// PSO���쐬�B
				m_pipelineStateObject[i].Init(m_descriptorHeaps[i]);
				// �V�F�[�_�[�e�[�u�����쐬�B
				m_shaderTable[i].Init(i, m_world, m_pipelineStateObject[i], m_descriptorHeaps[i]); 
			}
			
			// �_�[�e�B�t���O���I�t�ɂ���B
			m_isDirty = false;
		}
		void Engine::Dispatch(RenderContext& rc)
		{
			if (m_world.GetNumInstance() == 0) {
				// �C���X�^���X���o�^����Ă��Ȃ��B
				return;
			}
			
			CommitRegistGeometry(rc);

			int backBufferNo = g_graphicsEngine->GetBackBufferIndex();
			// ���C�g�����[���h���\�z����B
			m_world.Build(rc);

			// �J�������X�V�B
			Camera cam;
			cam.pos = g_camera3D->GetPosition();
			cam.mViewProjInv = g_camera3D->GetViewProjectionMatrixInv();
			cam.aspect = g_camera3D->GetAspect();
			cam.fNear = g_camera3D->GetNear();
			cam.fFar = g_camera3D->GetFar();
			m_rayGenerationCB[backBufferNo].CopyToVRAM(cam);

			// �g���X�g���N�`���[�h�o�b�t�@���X�V�B
			m_expandSRV[backBufferNo]->m_structuredBuffer.Update(m_expandSRV[backBufferNo]->m_srcData);

			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Transition.pResource = m_outputResource.Get();
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			rc.ResourceBarrier(barrier);

			D3D12_DISPATCH_RAYS_DESC raytraceDesc = {};
			raytraceDesc.Width = g_graphicsEngine->GetFrameBufferWidth();
			raytraceDesc.Height = g_graphicsEngine->GetFrameBufferHeight();
			raytraceDesc.Depth = 1;


			auto shaderTableEntrySize = m_shaderTable[backBufferNo].GetShaderTableEntrySize();
			auto numRayGenShader = m_shaderTable[backBufferNo].GetNumRayGenShader();
			auto numMissShader = m_shaderTable[backBufferNo].GetNumMissShader();
			auto numHitShader = m_shaderTable[backBufferNo].GetNumHitShader();

			// ���C�����V�F�[�_�[�̃V�F�[�_�[�e�[�u���̊J�n�A�h���X�ƃT�C�Y��ݒ�B
			raytraceDesc.RayGenerationShaderRecord.StartAddress = m_shaderTable[backBufferNo].GetGPUVirtualAddress();
			raytraceDesc.RayGenerationShaderRecord.SizeInBytes = shaderTableEntrySize;

			// �~�X�V�F�[�_�[�̃V�F�[�_�[�e�[�u���̊J�n�A�h���X�ƃT�C�Y��ݒ�B
			size_t missOffset = numRayGenShader * shaderTableEntrySize;
			raytraceDesc.MissShaderTable.StartAddress = m_shaderTable[backBufferNo].GetGPUVirtualAddress() + missOffset;
			raytraceDesc.MissShaderTable.StrideInBytes = shaderTableEntrySize;
			raytraceDesc.MissShaderTable.SizeInBytes = shaderTableEntrySize * numMissShader;

			// �q�b�g�O���[�v�V�F�[�_�[�̊J�n�A�h���X�ƃT�C�Y��ݒ�B
			size_t hitOffset = (numRayGenShader + numMissShader) * shaderTableEntrySize;
			raytraceDesc.HitGroupTable.StartAddress = m_shaderTable[backBufferNo].GetGPUVirtualAddress() + hitOffset;
			raytraceDesc.HitGroupTable.StrideInBytes = shaderTableEntrySize;
			raytraceDesc.HitGroupTable.SizeInBytes = shaderTableEntrySize * numHitShader * m_world.GetNumInstance();

			// �O���[�o�����[�g�V�O�l�`����ݒ�B
			rc.SetComputeRootSignature(m_pipelineStateObject[backBufferNo].GetGlobalRootSignature());

			// Dispatch
			//�O���[�o�����[�g�V�O�l�`�`���ɓo�^����Ă���f�B�X�N���v�^�q�[�v��o�^����B
			const DescriptorHeap* descriptorHeaps[] = {
				&m_descriptorHeaps[backBufferNo].GetSrvUavCbvDescriptorHeap(),
				&m_descriptorHeaps[backBufferNo].GetSamplerDescriptorHeap()
			};
			rc.SetDescriptorHeaps(ARRAYSIZE(descriptorHeaps), descriptorHeaps);

			rc.SetPipelineState(m_pipelineStateObject[backBufferNo]);
			rc.DispatchRays(raytraceDesc);

			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Transition.pResource = m_outputResource.Get();
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
			rc.ResourceBarrier(barrier);
		}
	}//namespace raytracing
}//namespace nsK2EngineLow 