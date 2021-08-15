#pragma once

namespace nsK2EngineLow {
	class ConstantBuffer;
	class Texture;
	class DescriptorHeap;
	class RenderTarget;

	namespace raytracing {
		class PSO;
	}
	/// <summary>
	/// �����_�����O�R���e�L�X�g�B
	/// </summary>
	class RenderContext : public Noncopyable {
	public:
		/// <summary>
		/// �������B
		/// </summary>
		/// <param name="commandList">�R�}���h���X�g�B</param>
		void Init(ID3D12GraphicsCommandList4* commandList)
		{
			m_commandList = commandList;
		}

		/// <summary>
		/// ���_�o�b�t�@��ݒ�B
		/// </summary>
		/// <param name="vb">���_�o�b�t�@�B</param>
		void SetVertexBuffer(VertexBuffer& vb)
		{
			m_commandList->IASetVertexBuffers(0, 1, &vb.GetView());
		}
		/// <summary>
		/// �C���f�b�N�X�o�b�t�@��ݒ�B
		/// </summary>
		/// <param name="ib"></param>
		void SetIndexBuffer(IndexBuffer& ib)
		{
			m_commandList->IASetIndexBuffer(&ib.GetView());
		}
		/// <summary>
		/// �v���~�e�B�u�̃g�|���W�[��ݒ�B
		/// </summary>
		/// <remarks>
		/// ID3D12GraphicsCommandList::��IASetPrimitiveTopology�̃��b�p�[�֐��B
		/// �ڍׂ�Microsoft�̃w���v���Q�ƁB
		/// </remarks>
		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology)
		{
			m_commandList->IASetPrimitiveTopology(topology);
		}
		/// <summary>
		/// �R�}���h���X�g��ݒ�B
		/// </summary>
		/// <param name="commandList">�R�}���h���X�g�B</param>
		void SetCommandList(ID3D12GraphicsCommandList4* commandList)
		{
			m_commandList = commandList;
		}
		/// <summary>
		/// �r���[�|�[�g�ƃV�U�����O��`���Z�b�g�Őݒ�
		/// </summary>
		/// <param name="viewport">�r���[�|�[�g</param>
		void SetViewportAndScissor(D3D12_VIEWPORT& viewport)
		{
			//�V�U�����O��`���ݒ肷��B
			D3D12_RECT scissorRect;
			scissorRect.bottom = static_cast<LONG>(viewport.Height);
			scissorRect.top = 0;
			scissorRect.left = 0;
			scissorRect.right = static_cast<LONG>(viewport.Width);
			SetScissorRect(scissorRect);

			m_commandList->RSSetViewports(1, &viewport);
			m_currentViewport = viewport;
		}
		/// <summary>
		/// �r���[�|�[�g���擾�B
		/// </summary>
		/// <returns></returns>
		D3D12_VIEWPORT GetViewport() const
		{
			return m_currentViewport;
		}
		/// <summary>
		/// �V�U�����O��`��ݒ�
		/// </summary>
		/// <param name="rect"></param>
		void SetScissorRect(D3D12_RECT& rect)
		{
			m_commandList->RSSetScissorRects(1, &rect);
		}

		/// <summary>
		/// ���[�g�V�O�l�`����ݒ�B
		/// </summary>
		void SetRootSignature(ID3D12RootSignature* rootSignature)
		{
			m_commandList->SetGraphicsRootSignature(rootSignature);
		}
		void SetRootSignature(RootSignature& rootSignature)
		{
			m_commandList->SetGraphicsRootSignature(rootSignature.Get());
		}
		void SetComputeRootSignature(ID3D12RootSignature* rootSignature)
		{
			m_commandList->SetComputeRootSignature(rootSignature);
		}
		void SetComputeRootSignature(RootSignature& rootSignature)
		{
			m_commandList->SetComputeRootSignature(rootSignature.Get());
		}
		/// <summary>
		/// �p�C�v���C���X�e�[�g��ݒ�B
		/// </summary>
		void SetPipelineState(ID3D12PipelineState* pipelineState)
		{
			m_commandList->SetPipelineState(pipelineState);
		}
		void SetPipelineState(PipelineState& pipelineState)
		{
			m_commandList->SetPipelineState(pipelineState.Get());
		}
		/// <summary>
		/// ���C�g���p�̃p�C�v���C���X�e�[�g�I�u�W�F�N�g��ݒ�B
		/// </summary>
		/// <param name="pso"></param>
		void SetPipelineState(raytracing::PSO& pso);

		/// <summary>
		/// �f�B�X�N���v�^�q�[�v��ݒ�B
		/// </summary>
		void SetDescriptorHeap(ID3D12DescriptorHeap* descHeap)
		{
			m_descriptorHeaps[0] = descHeap;
			m_commandList->SetDescriptorHeaps(1, m_descriptorHeaps);
		}

		void SetDescriptorHeap(DescriptorHeap& descHeap);
		void SetComputeDescriptorHeap(DescriptorHeap& descHeap);
		/// <summary>
		/// �����̃f�B�X�N���v�^�q�[�v��o�^�B
		/// </summary>
		/// <param name="numDescriptorHeap">�f�B�X�N���v�^�q�[�v�̐��B</param>
		/// <param name="descHeaps">�f�B�X�N���v�^�q�[�v�̔z��</param>
		void SetDescriptorHeaps(int numDescriptorHeap, const DescriptorHeap* descHeaps[])
		{
			for (int i = 0; i < numDescriptorHeap; i++) {
				m_descriptorHeaps[i] = descHeaps[i]->Get();
			}
			m_commandList->SetDescriptorHeaps(numDescriptorHeap, m_descriptorHeaps);
		}
		/// <summary>
		/// �萔�o�b�t�@��ݒ�B
		/// </summary>
		/// <param name="registerNo">�ݒ肷�郌�W�X�^�̔ԍ��B</param>
		/// <param name="cb">�萔�o�b�t�@�B</param>
		void SetConstantBuffer(int registerNo, ConstantBuffer& cb)
		{
			if (registerNo < MAX_CONSTANT_BUFFER) {
				m_constantBuffers[registerNo] = &cb;
			}
			else {
				//�͈͊O�A�N�Z�X�B
				std::abort();
			}
		}
		/// <summary>
		/// �V�F�[�_�[���\�[�X��ݒ�B
		/// </summary>
		/// <param name="registerNo">�ݒ肷�郌�W�X�^�̔ԍ��B</param>
		/// <param name="texture">�e�N�X�`���B</param>
		void SetShaderResource(int registerNo, Texture& texture)
		{
			if (registerNo < MAX_SHADER_RESOURCE) {
				m_shaderResources[registerNo] = &texture;
			}
			else {
				//�͈͊O�A�N�Z�X�B
				std::abort();
			}
		}
		/// <summary>
		/// �������̃����_�����O�^�[�Q�b�g��ݒ肷��B
		/// </summary>
		/// <remarks>
		/// MRT�𗘗p���������_�����O���s�������ꍇ�ɗ��p���Ă��������B
		/// </remarks>
		/// <param name="numRT">�����_�����O�^�[�Q�b�g�̐�</param>
		/// <param name="renderTarget">�����_�����O�^�[�Q�b�g�̔z��B</param>
		void SetRenderTargets(UINT numRT, RenderTarget* renderTargets[]);
		void SetRenderTargets(UINT numRT, RenderTarget* renderTargets[], D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);
		/// <summary>
		/// �����_�����O�^�[�Q�b�g��ݒ肷��B
		/// </summary>
		/// <param name="renderTarget"></param>
		void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle)
		{
			m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
		}
		/// <summary>
		/// �����_�����O�^�[�Q�b�g���X���b�g0�ɐݒ肷��B
		/// </summary>
		/// <remarks>
		/// �{�֐��̓r���[�|�[�g�̐ݒ���s���܂���B
		/// ���[�U�[���œK�؂ȃr���[�|�[�g���w�肷��K�v������܂��B
		/// </remarks>
		/// <param name="renderTarget">�����_�����O�^�[�Q�b�g</param>
		void SetRenderTarget(RenderTarget& renderTarget)
		{
			RenderTarget* rtArray[] = { &renderTarget };
			SetRenderTargets(1, rtArray);
		}

		/// <summary>
		/// �����_�����O�^�[�Q�b�g�ƃr���[�|�[�g�𓯎��ɐݒ肷��B
		/// </summary>
		/// <remarks>
		/// ���̊֐��𗘗p����ƃ����_�����O�^�[�Q�b�g�Ɠ������ƍ����̃r���[�|�[�g���ݒ肳��܂��B
		/// </remarks>
		/// <param name="renderTarget">�����_�����O�^�[�Q�b�g</param>
		void SetRenderTargetAndViewport(RenderTarget& renderTarget);
		/// <summary>
		/// �������̃����_�����O�^�[�Q�b�g�ƃr���[�|�[�g�𓯎��ɐݒ肷��B
		/// </summary>
		/// /// <remarks>
		/// ���̊֐��𗘗p����ƃ����_�����O�^�[�Q�b�g�Ɠ������ƍ����̃r���[�|�[�g���ݒ肳��܂��B
		/// </remarks>
		/// <param name="numRT">�ݒ肷�郌���_�����O�^�[�Q�b�g�̖���</param>
		/// <param name="renderTargets">�����_�����O�^�[�Q�b�g�̔z��B</param>
		void SetRenderTargetsAndViewport(UINT numRT, RenderTarget* renderTargets[]);
		/// <summary>
		/// �������̃����_�����O�^�[�Q�b�g���N���A�B
		/// </summary>
		/// <remarks>
		/// �N���A�J���[�̓����_�����O�^�[�Q�b�g�̏��������Ɏw�肵���J���[�ł��B
		/// </remarks>
		/// <param name="numRt">�����_�����O�^�[�Q�b�g�̐�</param>
		/// <param name="renderTargets">�����_�����O�^�[�Q�b�g�̐�</param>
		void ClearRenderTargetViews(
			int numRt,
			RenderTarget* renderTargets[]
		);
		/// <summary>
		/// �����_�����O�^�[�Q�b�g�̃N���A�B
		/// </summary>
		/// <param name="rtvHandle">CPU�̃����_�����O�^�[�Q�b�g�r���[�̃f�B�X�N���v�^�n���h��</param>
		/// <param name="clearColor">�N���A�J���[</param>
		void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, const float* clearColor)
		{
			m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		}
		/// <summary>
		/// �����_�����O�^�[�Q�b�g�̃N���A�B
		/// </summary>
		/// <param name="renderTarget"></param>
		void ClearRenderTargetView(RenderTarget& renderTarget)
		{
			RenderTarget* rtArray[] = { &renderTarget };
			ClearRenderTargetViews(1, rtArray);
		}
		/// <summary>
		/// �f�v�X�X�e���V���r���[���N���A
		/// </summary>
		/// <param name="renderTarget">�����_�����O�^�[�Q�b�g</param>
		/// <param name="clearValue">�N���A�l</param>
		void ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, float clearValue)
		{
			m_commandList->ClearDepthStencilView(
				dsvHandle,
				D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
				clearValue,
				0,
				0,
				nullptr);
		}
		/// <summary>
		/// �����_�����O�^�[�Q�b�g�ւ̕`�����ݑ҂��B
		/// </summary>
		/// <remarks>
		/// �����_�����O�^�[�Q�b�g�Ƃ��Ďg���Ă���e�N�X�`�����V�F�[�_�[���\�[�X�r���[�Ƃ���
		/// �g�p�������ꍇ�́A���̊֐����g���ĕ`�����݊����҂����s���K�v������܂��B
		/// </remarks>
		/// <param name="renderTarget">�����_�����O�^�[�Q�b�g</param>
		void WaitUntilFinishDrawingToRenderTargets(int numRt, RenderTarget* renderTargets[]);
		void WaitUntilFinishDrawingToRenderTarget(RenderTarget& renderTarget);
		void WaitUntilFinishDrawingToRenderTarget(ID3D12Resource* renderTarget)
		{
			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				renderTarget,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT);
			m_commandList->ResourceBarrier(1, &barrier);
		}
		/// <summary>
		/// �����_�����O�^�[�Q�b�g�Ƃ��Ďg�p�\�ɂȂ�܂ő҂B
		/// </summary>
		/// <remarks>
		/// �����_�����O�^�[�Q�b�g�Ƃ��Đݒ肵�����ꍇ�́A
		/// �{�֐����g���Ďg�p�\�ɂȂ�܂őҋ@����K�v������܂��B
		/// </remarks>
		void WaitUntilToPossibleSetRenderTargets(int numRt, RenderTarget* renderTargets[]);
		void WaitUntilToPossibleSetRenderTarget(RenderTarget& renderTarget);
		void WaitUntilToPossibleSetRenderTarget(ID3D12Resource* renderTarget)
		{
			auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			m_commandList->ResourceBarrier(1, &barrier);
		}
		/// <summary>
		/// ���\�[�X�o���A�B
		/// </summary>
		/// <param name="barrier"></param>
		void ResourceBarrier(D3D12_RESOURCE_BARRIER& barrier)
		{
			m_commandList->ResourceBarrier(1, &barrier);
		}
		/// <summary>
		/// ���\�[�X�X�e�[�g��J�ڂ���B
		/// </summary>
		/// <param name="resrouce"></param>
		/// <param name="beforeState"></param>
		/// <param name="afterState"></param>
		void TransitionResourceState(ID3D12Resource* resrouce, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
		{
			auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resrouce, beforeState, afterState);
			m_commandList->ResourceBarrier(1, &barrier);
		}
		/// <summary>
		/// �R�}���h���X�g�����
		/// </summary>
		void Close()
		{
			m_commandList->Close();
		}
		/// <summary>
		/// �R�}���h���X�g�����Z�b�g�B
		/// </summary>
		/// <param name="commandAllocator"></param>
		/// <param name="pipelineState"></param>
		void Reset(ID3D12CommandAllocator* commandAllocator, ID3D12PipelineState* pipelineState)
		{
			m_commandList->Reset(commandAllocator, pipelineState);
			//�X�N���b�`���\�[�X���N���A�B
			m_scratchResourceList.clear();
		}
		/// <summary>
		/// �C���f�b�N�X���v���~�e�B�u��`��B
		/// </summary>
		/// <param name="indexCount">�C���f�b�N�X�̐��B</param>
		void DrawIndexed(UINT indexCount)
		{
			m_commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
		}
		/// <summary>
		/// �C���f�b�N�X���v���~�e�B�u���C���X�^���V���O�`��B
		/// </summary>
		/// <param name="indexCount">�C���f�b�N�X�̐��B</param>
		/// <param name="numInstance">�C���X�^���X�̐��B</param>
		void DrawIndexedInstance(UINT indexCount, UINT numInstance)
		{
			m_commandList->DrawIndexedInstanced(indexCount, numInstance, 0, 0, 0);
		}
		/// <summary>
		/// �R���s���[�g�V�F�[�_�[���f�B�X�p�b�`�B
		/// </summary>
		/// <param name="ThreadGroupCountX"></param>
		/// <param name="ThreadGroupCountY"></param>
		/// <param name="ThreadGroupCountZ"></param>
		void Dispatch(
			UINT ThreadGroupCountX,
			UINT ThreadGroupCountY,
			UINT ThreadGroupCountZ)
		{
			m_commandList->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
		}
		/// <summary>
		/// GPU�Ń��C�g���[�V���O�A�N�Z�����[�V�����\���̃r���h���s���܂��B
		/// </summary>
		void BuildRaytracingAccelerationStructure(D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc)
		{
			m_commandList->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);
		}
		/// <summary>
		/// ���C���f�B�X�p�b�`
		/// </summary>
		/// <param name="rayDesc"></param>
		void DispatchRays(D3D12_DISPATCH_RAYS_DESC& rayDesc)
		{
			m_commandList->DispatchRays(&rayDesc);
		}
		/// <summary>
		/// ���\�[�X���R�s�[�B
		/// </summary>
		/// <param name="pDst">�R�s�[��̃��\�[�X</param>
		/// <param name="pSrc">�R�s�[���̃��\�[�X</param>
		void CopyResource(ID3D12Resource* pDst, ID3D12Resource* pSrc)
		{
			m_commandList->CopyResource(pDst, pSrc);
		}


	private:

		/// <summary>
		/// �f�B�X�N���v�^�e�[�u����ݒ�B
		/// </summary>
		/// <param name="RootParameterIndex"></param>
		/// <param name="BaseDescriptor"></param>
		void SetGraphicsRootDescriptorTable(
			UINT RootParameterIndex,
			D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor)
		{
			m_commandList->SetGraphicsRootDescriptorTable(
				RootParameterIndex,
				BaseDescriptor
			);
		}
		/// <summary>
		/// �f�B�X�N���v�^�e�[�u����ݒ�B
		/// </summary>
		/// <param name="RootParameterIndex"></param>
		/// <param name="BaseDescriptor"></param>
		void SetComputeRootDescriptorTable(
			UINT RootParameterIndex,
			D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor)
		{
			m_commandList->SetComputeRootDescriptorTable(
				RootParameterIndex,
				BaseDescriptor
			);
		}
	private:
		enum { MAX_DESCRIPTOR_HEAP = 4 };	//�f�B�X�N���v�^�q�[�v�̍ő吔�B
		enum { MAX_CONSTANT_BUFFER = 8 };	//�萔�o�b�t�@�̍ő吔�B����Ȃ��Ȃ����瑝�₵�ĂˁB
		enum { MAX_SHADER_RESOURCE = 16 };	//�V�F�[�_�[���\�[�X�̍ő吔�B����Ȃ��Ȃ����瑝�₵�ĂˁB

		D3D12_VIEWPORT m_currentViewport;				//���݂̃r���[�|�[�g�B
		ID3D12GraphicsCommandList4* m_commandList;	//�R�}���h���X�g�B
		ID3D12DescriptorHeap* m_descriptorHeaps[MAX_DESCRIPTOR_HEAP];			//�f�B�X�N���v�^�q�[�v�̔z��B
		ConstantBuffer* m_constantBuffers[MAX_CONSTANT_BUFFER] = { nullptr };	//�萔�o�b�t�@�̔z��B
		Texture* m_shaderResources[MAX_SHADER_RESOURCE] = { nullptr };			//�V�F�[�_�[���\�[�X�̔z��B
		std::vector< ComPtr<ID3D12Resource> > m_scratchResourceList;			//�X�N���b�`���\�[�X�̃��X�g�B
	};
}
