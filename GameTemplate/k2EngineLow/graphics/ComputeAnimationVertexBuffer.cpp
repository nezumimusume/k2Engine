#include "k2EngineLowPreCompile.h"
#include "graphics/ComputeAnimationVertexBuffer.h"


namespace nsK2EngineLow {
	namespace {
		const int NUM_SRV_ONE_DISPATCH = 1; // 1回のディスパッチで使用されるSRVの数
		const int NUM_UAV_ONE_DISPATCH = 1; // 1回のディスパッチで使用されるUAVの数
	}
	void ComputeAnimationVertexBuffer::Init(
		const char* tkmFilePath,
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
				(void*)&mesh.vertexBuffer[0]
			);
			// アニメーション済み頂点を記憶するためのバッファを作成。
			 m_animatedVertexBufferArray[meshNo].Init(m_animatedVertexBufferRWSBArray[meshNo]);
			
			meshNo++;
		});

		// シェーダーをロード。
		m_shader.LoadCS("Assets/shader/ComputeAnimationVertexBuffer.fx", "CSMain");

		// ルートシグネチャを初期化。
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
		int numSrvTotal = NUM_SRV_ONE_DISPATCH * m_numMesh;
		int numUavTotal = NUM_UAV_ONE_DISPATCH * m_numMesh;
		
		// SRVとUAVの登録できるサイズをリサイズ
		m_descriptorHeap.ResizeShaderResource(numSrvTotal);
		m_descriptorHeap.ResizeUnorderAccessResource(numUavTotal);

		// ディスクリプタヒープを構築していく。
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

		// アニメーション済み頂点の計算処理をディスパッチ
		/*rc.SetComputeRootSignature(m_rootSignature);
		rc.SetComputeDescriptorHeap(m_descriptorHeap);
		rc.SetPipelineState(m_pipelineState);*/
		
		EndGPUEvent();
	}
}