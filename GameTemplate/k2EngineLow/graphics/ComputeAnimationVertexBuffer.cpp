#include "k2EngineLowPreCompile.h"
#include "graphics/ComputeAnimationVertexBuffer.h"


namespace nsK2EngineLow {
	using namespace std;
	namespace {
		const int NUM_CB_ONE_DISPATCH = 1;		// 1回のディスパッチで使用される定数バッファの数。
		const int NUM_SRV_ONE_DISPATCH = 3;		// 1回のディスパッチで使用されるSRVの数
		const int NUM_UAV_ONE_DISPATCH = 1;		// 1回のディスパッチで使用されるUAVの数
		const int NUM_THREAD_X_IN_GROUP = 256;	// 1スレッドグループに含まれるスレッドの数(X方向)。
		const int NUM_THREAD_Y_IN_GROUP = 4;	// 1スレッドグループに含まれるスレッドの数(Y方向)
	}
	void ComputeAnimationVertexBuffer::Init(
		const char* tkmFilePath,
		int numBone,
		void* boneMatrixArray,
		EnModelUpAxis enModelUpAxis,
		int numInstance,
		StructuredBuffer* worldMatrixArraySB
	){
		m_numInstance = numInstance;
		// tkmファイルをロード。
		m_tkmFile.Load(tkmFilePath, false, false, false);
		m_numMesh = m_tkmFile.GetNumMesh();
		m_meshArray = make_unique<SMeshLocal[]>(m_numMesh);
		
		int meshNo = 0;
		m_tkmFile.QueryMeshParts([&](const TkmFile::SMesh& mesh) {
			SMeshLocal& meshLocal = m_meshArray[meshNo];
			int numVertex = (int)mesh.vertexBuffer.size();
			int vertexStride = sizeof(TkmFile::SVertex);
			// アニメーション前の頂点を記憶する頂点バッファを作成。
			meshLocal.m_vertexBuffer.Init(vertexStride * numVertex, vertexStride);
			meshLocal.m_vertexBuffer.Copy((void*)&mesh.vertexBuffer[0]);
			// アニメーション前の頂点バッファのStructuredBufferを作成。
			meshLocal.m_vertexBufferSB.Init(meshLocal.m_vertexBuffer, false);
			// インデックスバッファを作成。
			int indexBufferArraySize = (int)max<size_t>(mesh.indexBuffer32Array.size(), mesh.indexBuffer16Array.size());
			meshLocal.m_animatedIndexBufferArray = make_unique<IndexBuffer[]>(indexBufferArraySize);
			meshLocal.m_originalIndexBufferSize  = make_unique<int[]>(indexBufferArraySize);
			meshLocal.m_numMaterial = mesh.materials.size();
			for (int indexBufferNo = 0; indexBufferNo < indexBufferArraySize; indexBufferNo++) {
				auto& indexBuffer = meshLocal.m_animatedIndexBufferArray[indexBufferNo];
				// インデックスのサイズ
				const int strideIndex = 4;
				// インデックスバッファをコピー。
				if (!mesh.indexBuffer32Array.empty()) {
					// 32ビットインデックスバッファ。
					int numIndex = (int)mesh.indexBuffer32Array[indexBufferNo].indices.size();
					// 一つのインスタンスのインデックスバッファのサイズ。
					int indexBufferSizeOne = numIndex * strideIndex;	
					// 全てのインスタンスのインデックスバッファのサイズ。
					int indexBufferSize = indexBufferSizeOne * numInstance;
					// インデックスバッファを初期化。
					indexBuffer.Init(indexBufferSize, strideIndex);
					// オリジナルのインデックスの数を記憶しておく。
					meshLocal.m_originalIndexBufferSize[indexBufferNo] = numIndex;
					for (int instanceNo = 0; instanceNo < numInstance; instanceNo++ ) {
						// インスタンスの数だけインデックスバッファをコピー。
						indexBuffer.Copy(
							(uint32_t*)&mesh.indexBuffer32Array[indexBufferNo].indices.at(0),
							numIndex,
							instanceNo * indexBufferSizeOne,
							instanceNo * numVertex
						);
					}
				}
				else {
					// 16ビットインデックスバッファ。
					int numIndex = (int)mesh.indexBuffer16Array[indexBufferNo].indices.size();
					// 一つのインスタンスのインデックスバッファのサイズ。
					int indexBufferSizeOne = numIndex * strideIndex;
					// 全てのインスタンスのインデックスバッファのサイズ。
					int indexBufferSize = indexBufferSizeOne * numInstance;
					// インデックスバッファを初期化。
					indexBuffer.Init(indexBufferSize, strideIndex);
					// オリジナルのインデックスの数を記憶しておく。
					meshLocal.m_originalIndexBufferSize[indexBufferNo] = numIndex;
					for (int instanceNo = 0; instanceNo < numInstance; instanceNo++) {
						// インスタンスの数だけインデックスバッファをコピー。
						indexBuffer.Copy(
							(uint16_t*)&mesh.indexBuffer16Array[indexBufferNo].indices.at(0),
							numIndex,
							instanceNo * indexBufferSizeOne,
							instanceNo * numVertex
						);
					}
				}
			}
			// アニメーション済み頂点バッファのRWStructuredBufferを初期化。
			meshLocal.m_animatedVertexBufferRWSB.Init(
				vertexStride,
				numVertex * numInstance,
				nullptr,
				false
			);
			// アニメーション済み頂点を記憶するためのバッファを作成。
			meshLocal.m_animatedVertexBuffer.Init(meshLocal.m_animatedVertexBufferRWSB);
			
			meshNo++;
		});

		// シェーダーをロード。
		const char* fxFilePath = "Assets/shader/preProcess/ComputeAnimationVertexBuffer.fx";
		const char* entryPoint = "CSMain";
		m_shader = g_engine->GetShaderFromBank(fxFilePath, entryPoint);
		if (m_shader == nullptr) {
			m_shader = new Shader;
			m_shader->LoadCS(fxFilePath, entryPoint);
			g_engine->RegistShaderToBank(fxFilePath, entryPoint, m_shader);
		}
		
		// ボーン行列を送るためのストラクチャードバッファを作成。
		m_boneMatrixArray = boneMatrixArray;
		m_boneMatricesStructureBuffer.Init(sizeof(Matrix), numBone, boneMatrixArray);

		
		for (int meshNo = 0; meshNo < m_numMesh; meshNo++) {
			auto& mesh = m_meshArray[meshNo];
			
			// 定数バッファを初期化。
			mesh.m_cb0.Init(sizeof(CB_0));
			// ルートシグネチャを初期化。
			mesh.m_rootSignature.Init(
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
			psoDesc.pRootSignature = mesh.m_rootSignature.Get();
			psoDesc.CS = CD3DX12_SHADER_BYTECODE(m_shader->GetCompiledBlob());
			psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
			psoDesc.NodeMask = 0;
			mesh.m_pipelineState.Init(psoDesc);
			
		}
		
		// ディスクリプタヒープを作成。
		CreateDescriptorHeaps(worldMatrixArraySB);
	}
	void ComputeAnimationVertexBuffer::CreateDescriptorHeaps(StructuredBuffer* worldMatrixArraySB)
	{
		// SRVとUAVの総数を計算する。
		int numCBTotal 	= NUM_CB_ONE_DISPATCH * m_numMesh;
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
			auto& mesh = m_meshArray[meshNo];
			m_descriptorHeap.RegistShaderResource(srvNo, mesh.m_vertexBufferSB);
			m_descriptorHeap.RegistShaderResource(srvNo + 1, m_boneMatricesStructureBuffer);
			if (worldMatrixArraySB) {
				m_descriptorHeap.RegistShaderResource(srvNo + 2, *worldMatrixArraySB);
			}
			m_descriptorHeap.RegistUnorderAccessResource(uavNo, mesh.m_animatedVertexBufferRWSB);
			m_descriptorHeap.RegistConstantBuffer(cbNo, mesh.m_cb0);

			srvNo += NUM_SRV_ONE_DISPATCH;
			uavNo += NUM_UAV_ONE_DISPATCH;
			cbNo += NUM_CB_ONE_DISPATCH;
		}
		m_descriptorHeap.Commit();
	}
	
	void ComputeAnimationVertexBuffer::Dispatch(RenderContext& rc, const Matrix& worldMatrix, int numInstance)
	{
		BeginGPUEvent("ComputeAnimationVertexBuffer");

		if (!m_isFirstDispatch) {
			for (int meshNo = 0; meshNo < m_numMesh; meshNo++) {
				rc.TransitionResourceState(
					m_meshArray[meshNo].m_animatedVertexBuffer.GetID3DResourceAddress(),
					D3D12_RESOURCE_STATE_GENERIC_READ,
					D3D12_RESOURCE_STATE_UNORDERED_ACCESS
				);
			}			
		}
		if (m_boneMatrixArray) {
			// ボーン行列が指定されている。
			m_boneMatricesStructureBuffer.Update(m_boneMatrixArray);
		}
		// アニメーション済み頂点の計算処理をディスパッチ
		for (int meshNo = 0; meshNo < m_numMesh; meshNo++) {
			auto& mesh = m_meshArray[meshNo];
			rc.SetComputeRootSignature(mesh.m_rootSignature);
			rc.SetComputeDescriptorHeap(m_descriptorHeap);
			rc.SetPipelineState(mesh.m_pipelineState);
			const int numVertex = mesh.m_vertexBuffer.GetNumVertex();
			const int numThreadGroup_x = (numVertex / NUM_THREAD_X_IN_GROUP) + 1;
			const int numThreadGroup_y = (numInstance / NUM_THREAD_Y_IN_GROUP) + 1;
			CB_0 cb0;
			cb0.numVertex = numVertex;
			cb0.worldMatrix = worldMatrix;
			cb0.outputOffset = numInstance;
			mesh.m_cb0.CopyToVRAM(&cb0);
			rc.Dispatch(numThreadGroup_x, numThreadGroup_y, 1);
		}
		
		// リソースステートを遷移させる。
		for (int meshNo = 0; meshNo < m_numMesh; meshNo++) {
			auto& mesh = m_meshArray[meshNo];
			rc.TransitionResourceState(
				mesh.m_animatedVertexBuffer.GetID3DResourceAddress(),
				D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
				D3D12_RESOURCE_STATE_GENERIC_READ
			);
		}
		m_isFirstDispatch = false;

		// ディスパッチしたインスタンスの数に応じて、インデックスバッファのカウントを決める。
		for (int meshNo = 0; meshNo < m_numMesh; meshNo++) {
			auto& mesh = m_meshArray[meshNo];
			for (int matNo = 0; matNo < mesh.m_numMaterial; matNo++) {
				int indexCount = mesh.m_originalIndexBufferSize[matNo] * numInstance;
				mesh.m_animatedIndexBufferArray[matNo].SetCount(indexCount);
			}
		}

		EndGPUEvent();
	}
	
}