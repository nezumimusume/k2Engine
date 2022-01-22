#include "k2EngineLowPreCompile.h"
#include "graphics/ComputeAnimationVertexBuffer.h"


namespace nsK2EngineLow {
	namespace {
		const int NUM_SRV_ONE_DISPATCH = 1; // 1��̃f�B�X�p�b�`�Ŏg�p�����SRV�̐�
		const int NUM_UAV_ONE_DISPATCH = 1; // 1��̃f�B�X�p�b�`�Ŏg�p�����UAV�̐�
	}
	void ComputeAnimationVertexBuffer::Init(
		const char* tkmFilePath,
		EnModelUpAxis enModelUpAxis
	){
		// tkm�t�@�C�������[�h�B
		m_tkmFile.Load(tkmFilePath, false, false, false);
		m_numMesh = m_tkmFile.GetNumMesh();
		m_vertexBufferArray = std::make_unique<VertexBuffer[]>(m_numMesh);
		m_vertexBufferSBArray = std::make_unique<StructuredBuffer[]>(m_numMesh);
		m_animatedVertexBufferArray = std::make_unique<VertexBuffer[]>(m_numMesh);
		m_animatedVertexBufferRWSBArray = std::make_unique<RWStructuredBuffer[]>(m_numMesh);
		m_rootSignatureArray = std::make_unique<RootSignature[]>(m_numMesh);
		m_pipilineStateArray = std::make_unique<PipelineState[]>(m_numMesh);

		int meshNo = 0;
		m_tkmFile.QueryMeshParts([&](const TkmFile::SMesh& mesh) {
			int numVertex = (int)mesh.vertexBuffer.size();
			int vertexStride = sizeof(TkmFile::SVertex);
			// �A�j���[�V�����O�̒��_���L�����钸�_�o�b�t�@���쐬�B
			m_vertexBufferArray[meshNo].Init(vertexStride * numVertex, vertexStride);
			m_vertexBufferArray[meshNo].Copy((void*)&mesh.vertexBuffer[0]);
			// �A�j���[�V�����O�̒��_�o�b�t�@��StructuredBuffer���쐬�B
			m_vertexBufferSBArray[meshNo].Init(m_vertexBufferArray[meshNo], false);

			// �A�j���[�V�����ςݒ��_�o�b�t�@��RWStructuredBuffer���������B
			m_animatedVertexBufferRWSBArray[meshNo].Init(
				vertexStride,
				numVertex,
				(void*)&mesh.vertexBuffer[0]
			);
			// �A�j���[�V�����ςݒ��_���L�����邽�߂̃o�b�t�@���쐬�B
			 m_animatedVertexBufferArray[meshNo].Init(m_animatedVertexBufferRWSBArray[meshNo]);
			
			meshNo++;
		});

		// �V�F�[�_�[�����[�h�B
		m_shader.LoadCS("Assets/shader/ComputeAnimationVertexBuffer.fx", "CSMain");

		// ���[�g�V�O�l�`�����������B
		for (int meshNo = 0; meshNo < m_numMesh; meshNo++) {
			m_rootSignatureArray[meshNo].Init(
				nullptr,
				0,
				0,
				NUM_SRV_ONE_DISPATCH,
				NUM_UAV_ONE_DISPATCH,
				0,
				NUM_SRV_ONE_DISPATCH * meshNo,
				NUM_UAV_ONE_DISPATCH * meshNo
			);

			// �p�C�v���C���X�e�[�g���������B
			D3D12_COMPUTE_PIPELINE_STATE_DESC  psoDesc = { 0 };
			psoDesc.pRootSignature = m_rootSignatureArray[meshNo].Get();
			psoDesc.CS = CD3DX12_SHADER_BYTECODE(m_shader.GetCompiledBlob());
			psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
			psoDesc.NodeMask = 0;
			m_pipilineStateArray[meshNo].Init(psoDesc);
		}
		
		// �f�B�X�N���v�^�q�[�v���쐬�B
		CreateDescriptorHeaps();
	}
	void ComputeAnimationVertexBuffer::CreateDescriptorHeaps()
	{
		// SRV��UAV�̑������v�Z����B
		int numSrvTotal = NUM_SRV_ONE_DISPATCH * m_numMesh;
		int numUavTotal = NUM_UAV_ONE_DISPATCH * m_numMesh;
		
		// SRV��UAV�̓o�^�ł���T�C�Y�����T�C�Y
		m_descriptorHeap.ResizeShaderResource(numSrvTotal);
		m_descriptorHeap.ResizeUnorderAccessResource(numUavTotal);

		// �f�B�X�N���v�^�q�[�v���\�z���Ă����B
		int srvNo = 0;
		int uavNo = 0;
		for (int meshNo = 0; meshNo < m_numMesh; meshNo++) {
			m_descriptorHeap.RegistShaderResource(srvNo, m_vertexBufferSBArray[meshNo]);
			m_descriptorHeap.RegistUnorderAccessResource(uavNo, m_animatedVertexBufferRWSBArray[meshNo]);
			srvNo++;
			uavNo++;
		}
		m_descriptorHeap.Commit();
	}
	void ComputeAnimationVertexBuffer::Dispatch(RenderContext& rc)
	{
		BeginGPUEvent("ComputeAnimationVertexBuffer");

		// �A�j���[�V�����ςݒ��_�̌v�Z�������f�B�X�p�b�`
		/*rc.SetComputeRootSignature(m_rootSignature);
		rc.SetComputeDescriptorHeap(m_descriptorHeap);
		rc.SetPipelineState(m_pipelineState);*/
		
		EndGPUEvent();
	}
}