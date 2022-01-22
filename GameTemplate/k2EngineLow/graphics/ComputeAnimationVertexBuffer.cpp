#include "k2EngineLowPreCompile.h"
#include "graphics/ComputeAnimationVertexBuffer.h"


namespace nsK2EngineLow {
	namespace {
		const int NUM_CB_ONE_DISPATCH = 1;		// 1回のディスパッチで使用される定数バッファの数。
		const int NUM_SRV_ONE_DISPATCH = 2;		// 1回のディスパッチで使用されるSRVの数
		const int NUM_UAV_ONE_DISPATCH = 1;		// 1回のディスパッチで使用されるUAVの数
		const int NUM_THREAD_IN_GROUP = 256;	// 1スレッドグループに含まれるスレッドの数。
	}
	void ComputeAnimationVertexBuffer::Init(
		const char* tkmFilePath,
		int numBone,
		void* boneMatrixArray,
		EnModelUpAxis enModelUpAxis
	){
		// tkmファイルをロード。
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
			// アニメーション前の頂点を記憶する頂点バッファを作成。
			m_vertexBufferArray[meshNo].Init(vertexStride * numVertex, vertexStride);
			m_vertexBufferArray[meshNo].Copy((void*)&mesh.vertexBuffer[0]);
			// アニメーション前の頂点バッファのStructuredBufferを作成。
			m_vertexBufferSBArray[meshNo].Init(m_vertexBufferArray[meshNo], false);

			// アニメーション済み頂点バッファのRWStructuredBufferを初期化。
			m_animatedVertexBufferRWSBArray[meshNo].Init(
				vertexStride,
				numVertex,
				nullptr,
				false
			);
			// アニメーション済み頂点を記憶するためのバッファを作成。
			 m_animatedVertexBufferArray[meshNo].Init(m_animatedVertexBufferRWSBArray[meshNo]);
			
			meshNo++;
		});

		// シェーダーをロード。
		m_shader.LoadCS("Assets/shader/ComputeAnimationVertexBuffer.fx", "CSMain");
		
		// ボーン行列を送るためのストラクチャードバッファを作成。
		m_boneMatrixArray = boneMatrixArray;
		m_boneMatricesStructureBuffer.Init(sizeof(Matrix), numBone, boneMatrixArray);

		// 定数バッファを初期化。
		for (int meshNo = 0; meshNo < m_numMesh; meshNo++) {
			m_cb0Array[meshNo].Init(sizeof(CB_0));
		}
		// ルートシグネチャを初期化。
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

			// パイプラインステートを初期化。
			D3D12_COMPUTE_PIPELINE_STATE_DESC  psoDesc = { 0 };
			psoDesc.pRootSignature = m_rootSignatureArray[meshNo].Get();
			psoDesc.CS = CD3DX12_SHADER_BYTECODE(m_shader.GetCompiledBlob());
			psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
			psoDesc.NodeMask = 0;
			m_pipilineStateArray[meshNo].Init(psoDesc);
		}
		
		// ディスクリプタヒープを作成。
		CreateDescriptorHeaps();
	}
	void ComputeAnimationVertexBuffer::CreateDescriptorHeaps()
	{
		// SRVとUAVの総数を計算する。
		int numCBTotal = NUM_CB_ONE_DISPATCH * m_numMesh;
		int numSrvTotal = NUM_SRV_ONE_DISPATCH * m_numMesh;
		int numUavTotal = NUM_UAV_ONE_DISPATCH * m_numMesh;
		
		// CB、SRV、UAVの登録できるサイズをリサイズ
		m_descriptorHeap.ResizeConstantBuffer(numCBTotal);
		m_descriptorHeap.ResizeShaderResource(numSrvTotal);
		m_descriptorHeap.ResizeUnorderAccessResource(numUavTotal);

		// ディスクリプタヒープを構築していく。
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
			// ボーン行列が指定されている。
			m_boneMatricesStructureBuffer.Update(m_boneMatrixArray);
			// アニメーション済み頂点の計算処理をディスパッチ
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
		// リソースステートを遷移させる。
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