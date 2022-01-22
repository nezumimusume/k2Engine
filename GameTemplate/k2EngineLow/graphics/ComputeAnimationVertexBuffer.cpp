#include "k2EngineLowPreCompile.h"
#include "graphics/ComputeAnimationVertexBuffer.h"


namespace nsK2EngineLow {
	namespace {
		const int NUM_CB_ONE_DISPATCH = 1;		// 1��̃f�B�X�p�b�`�Ŏg�p�����萔�o�b�t�@�̐��B
		const int NUM_SRV_ONE_DISPATCH = 2;		// 1��̃f�B�X�p�b�`�Ŏg�p�����SRV�̐�
		const int NUM_UAV_ONE_DISPATCH = 1;		// 1��̃f�B�X�p�b�`�Ŏg�p�����UAV�̐�
		const int NUM_THREAD_IN_GROUP = 256;	// 1�X���b�h�O���[�v�Ɋ܂܂��X���b�h�̐��B
	}
	void ComputeAnimationVertexBuffer::Init(
		const char* tkmFilePath,
		int numBone,
		void* boneMatrixArray,
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
		m_cb0Array = std::make_unique<ConstantBuffer[]>(m_numMesh);
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
				nullptr,
				false
			);
			// �A�j���[�V�����ςݒ��_���L�����邽�߂̃o�b�t�@���쐬�B
			 m_animatedVertexBufferArray[meshNo].Init(m_animatedVertexBufferRWSBArray[meshNo]);
			
			meshNo++;
		});

		// �V�F�[�_�[�����[�h�B
		m_shader.LoadCS("Assets/shader/ComputeAnimationVertexBuffer.fx", "CSMain");
		
		// �{�[���s��𑗂邽�߂̃X�g���N�`���[�h�o�b�t�@���쐬�B
		m_boneMatrixArray = boneMatrixArray;
		m_boneMatricesStructureBuffer.Init(sizeof(Matrix), numBone, boneMatrixArray);

		// �萔�o�b�t�@���������B
		for (int meshNo = 0; meshNo < m_numMesh; meshNo++) {
			m_cb0Array[meshNo].Init(sizeof(CB_0));
		}
		// ���[�g�V�O�l�`�����������B
		for (int meshNo = 0; meshNo < m_numMesh; meshNo++) {
			m_rootSignatureArray[meshNo].Init(
				nullptr,
				0,
				NUM_CB_ONE_DISPATCH,
				NUM_SRV_ONE_DISPATCH,
				NUM_UAV_ONE_DISPATCH,
				NUM_CB_ONE_DISPATCH * meshNo,
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
		int numCBTotal = NUM_CB_ONE_DISPATCH * m_numMesh;
		int numSrvTotal = NUM_SRV_ONE_DISPATCH * m_numMesh;
		int numUavTotal = NUM_UAV_ONE_DISPATCH * m_numMesh;
		
		// CB�ASRV�AUAV�̓o�^�ł���T�C�Y�����T�C�Y
		m_descriptorHeap.ResizeConstantBuffer(numCBTotal);
		m_descriptorHeap.ResizeShaderResource(numSrvTotal);
		m_descriptorHeap.ResizeUnorderAccessResource(numUavTotal);

		// �f�B�X�N���v�^�q�[�v���\�z���Ă����B
		int cbNo = 0;
		int srvNo = 0;
		int uavNo = 0;
		for (int meshNo = 0; meshNo < m_numMesh; meshNo++) {
			m_descriptorHeap.RegistShaderResource(srvNo, m_vertexBufferSBArray[meshNo]);
			m_descriptorHeap.RegistShaderResource(srvNo + 1, m_boneMatricesStructureBuffer);
			m_descriptorHeap.RegistUnorderAccessResource(uavNo, m_animatedVertexBufferRWSBArray[meshNo]);
			m_descriptorHeap.RegistConstantBuffer(cbNo, m_cb0Array[meshNo]);
			srvNo += NUM_SRV_ONE_DISPATCH;
			uavNo += NUM_UAV_ONE_DISPATCH;
			cbNo += NUM_CB_ONE_DISPATCH;
		}
		m_descriptorHeap.Commit();
	}
	void ComputeAnimationVertexBuffer::Dispatch(RenderContext& rc)
	{
		BeginGPUEvent("ComputeAnimationVertexBuffer");

		if (!m_isFirstDispatch) {
			for (int meshNo = 0; meshNo < m_numMesh; meshNo++) {
				rc.TransitionResourceState(
					m_animatedVertexBufferArray[meshNo].GetID3DResourceAddress(),
					D3D12_RESOURCE_STATE_GENERIC_READ,
					D3D12_RESOURCE_STATE_UNORDERED_ACCESS
				);
			}
			m_isFirstDispatch = false;
		}
		if (m_boneMatrixArray) {
			// �{�[���s�񂪎w�肳��Ă���B
			m_boneMatricesStructureBuffer.Update(m_boneMatrixArray);
			// �A�j���[�V�����ςݒ��_�̌v�Z�������f�B�X�p�b�`
			for (int meshNo = 0; meshNo < m_numMesh; meshNo++) {
				rc.SetComputeRootSignature(m_rootSignatureArray[meshNo]);
				rc.SetComputeDescriptorHeap(m_descriptorHeap);
				rc.SetPipelineState(m_pipilineStateArray[meshNo]);
				const int numVertex = m_vertexBufferArray[meshNo].GetNumVertex();
				const int numThreadGround = (numVertex / NUM_THREAD_IN_GROUP) + 1;
				CB_0 cb0;
				cb0.numVertex = numVertex;
				m_cb0Array[meshNo].CopyToVRAM(&cb0);
				rc.Dispatch(numThreadGround, 1, 1);

			}
		}
		// ���\�[�X�X�e�[�g��J�ڂ�����B
		for (int meshNo = 0; meshNo < m_numMesh; meshNo++) {
			rc.TransitionResourceState(
				m_animatedVertexBufferArray[meshNo].GetID3DResourceAddress(),
				D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
				D3D12_RESOURCE_STATE_GENERIC_READ
			);
		}
		EndGPUEvent();
	}
}