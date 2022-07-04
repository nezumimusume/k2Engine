#include "k2EngineLowPreCompile.h"
#include "MeshParts.h"
#include "Skeleton.h"
#include "Material.h"
#include "IndexBuffer.h"
#include "ComputeAnimationVertexBuffer.h"

namespace nsK2EngineLow {
	static TResourceBank<Material>& GetMaterialBank()
	{
		static TResourceBank<Material> materialBank;
		return materialBank;
	}
	MeshParts::~MeshParts()
	{
		for (auto& mesh : m_meshs) {
			//インデックスバッファを削除。
			for (auto& ib : mesh->m_indexBufferArray) {
				delete ib;
			}
			//メッシュを削除。
			delete mesh;
		}
	}
	void MeshParts::InitFromTkmFile(
		const TkmFile& tkmFile,
		const char* fxFilePath,
		const char* vsEntryPointFunc,
		const char* vsSkinEntryPointFunc,
		const char* psEntryPointFunc,
		void* expandData,
		int expandDataSize,
		const std::array<IShaderResource*, MAX_MODEL_EXPAND_SRV>& expandShaderResourceView,
		const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat,
		AlphaBlendMode alphaBlendMode,
		bool isDepthWrite,
		bool isDepthTest,
		D3D12_CULL_MODE cullMode,
		ComputeAnimationVertexBuffer* computedAnimationVertexBuffer
	)
	{
		m_meshs.resize(tkmFile.GetNumMesh());
		int meshNo = 0;
		int materianNo = 0;
		tkmFile.QueryMeshParts([&](const TkmFile::SMesh& mesh) {
			//tkmファイルのメッシュ情報からメッシュを作成する。
			CreateMeshFromTkmMesh(
				mesh,
				meshNo,
				materianNo,
				fxFilePath,
				vsEntryPointFunc,
				vsSkinEntryPointFunc,
				psEntryPointFunc,
				colorBufferFormat,
				alphaBlendMode,
				isDepthWrite,
				isDepthTest,
				cullMode,
				computedAnimationVertexBuffer
			);
			meshNo++;
		});
		//共通定数バッファの作成。
		m_commonConstantBuffer.Init(sizeof(SConstantBuffer), nullptr);
		//ユーザー拡張用の定数バッファを作成。
		if (expandData) {
			m_expandConstantBuffer.Init(expandDataSize, nullptr);
			m_expandData = expandData;
		}
		for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
			m_expandShaderResourceView[i] = expandShaderResourceView[i];
		}
		//ディスクリプタヒープを作成。
		CreateDescriptorHeaps();
	}
	void MeshParts::ReInitMaterials(const MaterialReInitData& reInitData)
	{
		for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
			m_expandShaderResourceView[i] = reInitData.m_expandShaderResoruceView[i];
		}
		//ディスクリプタヒープを作成。
		CreateDescriptorHeaps();
	}
	void MeshParts::CreateDescriptorHeaps()
	{
		// 必要なディスクリプタヒープの総数を計算する。
		int srvNo = 0;
		int cbNo = 0;
		for (auto& mesh : m_meshs) {
			for (int matNo = 0; matNo < mesh->m_materials.size(); matNo++) {
				srvNo += NUM_SRV_ONE_MATERIAL;
				cbNo += NUM_CBV_ONE_MATERIAL;
			}
		}
		// シェーダーリソースビューと定数バッファの登録できるサイズをリサイズする。
		m_descriptorHeap.ResizeShaderResource(srvNo);
		m_descriptorHeap.ResizeConstantBuffer(cbNo);
		// UAVいらない。
		m_descriptorHeap.ResizeUnorderAccessResource(0);
		//ディスクリプタヒープを構築していく。
		srvNo = 0;
		cbNo = 0;
		for (auto& mesh : m_meshs) {
			for (int matNo = 0; matNo < mesh->m_materials.size(); matNo++) {

				//ディスクリプタヒープにディスクリプタを登録していく。
				m_descriptorHeap.RegistShaderResource(srvNo, mesh->m_materials[matNo]->GetAlbedoMap());		//アルベドマップ。
				m_descriptorHeap.RegistShaderResource(srvNo + 1, mesh->m_materials[matNo]->GetNormalMap());		//法線マップ。
				m_descriptorHeap.RegistShaderResource(srvNo + 2, mesh->m_materials[matNo]->GetSpecularMap());		//スペキュラマップ。
				m_descriptorHeap.RegistShaderResource(srvNo + 3, m_boneMatricesStructureBuffer);							//ボーンのストラクチャードバッファ。
				for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
					if (m_expandShaderResourceView[i]) {
						m_descriptorHeap.RegistShaderResource(srvNo + EXPAND_SRV_REG__START_NO + i, *m_expandShaderResourceView[i]);
					}
				}
				srvNo += NUM_SRV_ONE_MATERIAL;
				m_descriptorHeap.RegistConstantBuffer(cbNo, m_commonConstantBuffer);
				if (m_expandConstantBuffer.IsValid()) {
					m_descriptorHeap.RegistConstantBuffer(cbNo + 1, m_expandConstantBuffer);
				}
				cbNo += NUM_CBV_ONE_MATERIAL;
			}
		}
		m_descriptorHeap.Commit();
	}
	void MeshParts::CreateMeshFromTkmMesh(
		const TkmFile::SMesh& tkmMesh,
		int meshNo,
		int& materialNum,
		const char* fxFilePath,
		const char* vsEntryPointFunc,
		const char* vsSkinEntryPointFunc,
		const char* psEntryPointFunc,
		const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat,
		AlphaBlendMode alphaBlendMode,
		bool isDepthWrite,
		bool isDepthTest,
		D3D12_CULL_MODE cullMode,
		ComputeAnimationVertexBuffer* computedAnimationVertexBuffer
	) {
		//1. 頂点バッファを作成。
		int numVertex = (int)tkmMesh.vertexBuffer.size();
		int vertexStride = sizeof(TkmFile::SVertex);
		auto mesh = new SMesh;
		mesh->skinFlags.reserve(tkmMesh.materials.size());

		mesh->m_vertexBuffer.Init(vertexStride * numVertex, vertexStride);
		mesh->m_vertexBuffer.Copy((void*)&tkmMesh.vertexBuffer[0]);
		/*
		if (dispatchComputedAnimationVertexBuffer) {
			// アニメーション済み頂点を記憶するためのバッファを作成。
			mesh->m_animatedVertexBuffer.Init(vertexStride * numVertex, vertexStride);
			mesh->m_animatedVertexBuffer.Copy((void*)&tkmMesh.vertexBuffer[0]);
			// アニメーション済み頂点バッファのRWStructuredBufferを初期化。
			mesh->m_animatedVertexBufferRWSB.Init(mesh->m_animatedVertexBuffer, false);
		}
		*/
		m_computedAnimationVertexBuffer = computedAnimationVertexBuffer;
		
		auto SetSkinFlag = [&](int index) {
			if (tkmMesh.vertexBuffer[index].skinWeights.x > 0.0f) {
				//スキンがある。
				mesh->skinFlags.push_back(1);
			}
			else {
				//スキンなし。
				mesh->skinFlags.push_back(0);
			}
		};
		//2. インデックスバッファを作成。
		if (!tkmMesh.indexBuffer16Array.empty()) {
			//インデックスのサイズが2byte
			mesh->m_indexBufferArray.reserve(tkmMesh.indexBuffer16Array.size());
			for (auto& tkIb : tkmMesh.indexBuffer16Array) {
				auto ib = new IndexBuffer;
				ib->Init(static_cast<int>(tkIb.indices.size()) * 2, 2);
				ib->Copy((uint16_t*)&tkIb.indices.at(0), 0, 0, 0);

				//スキンがあるかどうかを設定する。
				SetSkinFlag(tkIb.indices[0]);

				mesh->m_indexBufferArray.push_back(ib);
			}
		}
		else {
			//インデックスのサイズが4byte
			mesh->m_indexBufferArray.reserve(tkmMesh.indexBuffer32Array.size());
			for (auto& tkIb : tkmMesh.indexBuffer32Array) {
				auto ib = new IndexBuffer;
				ib->Init(static_cast<int>(tkIb.indices.size()) * 4, 4);
				ib->Copy((uint32_t*)&tkIb.indices.at(0), 0, 0, 0);

				//スキンがあるかどうかを設定する。
				SetSkinFlag(tkIb.indices[0]);

				mesh->m_indexBufferArray.push_back(ib);
			}
		}
		//3. マテリアルを作成。
		auto& materialBank = GetMaterialBank();
		mesh->m_materials.reserve(tkmMesh.materials.size());
		for (auto& tkmMat : tkmMesh.materials) {
			char materiayKey[MAX_PATH];
			sprintf_s(
				materiayKey,
				MAX_PATH,
				"%s, %s, %s, %s, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %s, %s, %s",
				fxFilePath,
				vsEntryPointFunc,
				vsSkinEntryPointFunc,
				psEntryPointFunc,
				(int)colorBufferFormat[0],
				(int)colorBufferFormat[1],
				(int)colorBufferFormat[2],
				(int)colorBufferFormat[3],
				(int)colorBufferFormat[4],
				(int)colorBufferFormat[5],
				(int)colorBufferFormat[6],
				(int)colorBufferFormat[7],
				alphaBlendMode,
				isDepthWrite,
				isDepthTest,
				cullMode,
				tkmMat.albedoMapFileName.empty() ? "none" : tkmMat.albedoMapFileName.c_str(),
				tkmMat.normalMapFileName.empty() ? "none" : tkmMat.normalMapFileName.c_str(),
				tkmMat.specularMapFileName.empty() ? "none" : tkmMat.specularMapFileName.c_str()
			);
			auto mat = materialBank.Get(materiayKey);
			if (mat == nullptr) {
				mat = new Material();

				mat->InitFromTkmMaterila(
					tkmMat,
					fxFilePath,
					vsEntryPointFunc,
					vsSkinEntryPointFunc,
					psEntryPointFunc,
					colorBufferFormat,
					NUM_SRV_ONE_MATERIAL,
					NUM_CBV_ONE_MATERIAL,
					NUM_CBV_ONE_MATERIAL * materialNum,
					NUM_SRV_ONE_MATERIAL * materialNum,
					alphaBlendMode,
					isDepthWrite,
					isDepthTest,
					cullMode
				);
				materialBank.Regist(materiayKey, mat);
			}
			//作成したマテリアル数をカウントする。
			materialNum++;
			mesh->m_materials.push_back(mat);
		}

		m_meshs[meshNo] = mesh;

	}
	const VertexBuffer& MeshParts::GetAnimatedVertexBuffer(int meshNo)  const
	{
		return m_computedAnimationVertexBuffer->GetAnimatedVertexBuffer(meshNo);
	}
	VertexBuffer& MeshParts::GetAnimatedVertexBuffer(int meshNo)
	{
		return m_computedAnimationVertexBuffer->GetAnimatedVertexBuffer(meshNo);
	}
	const IndexBuffer& MeshParts::GetAnimatedIndexBuffer(int meshNo, int matNo) const
	{
		return m_computedAnimationVertexBuffer->GetAnimatedIndexBuffer(meshNo, matNo);
	}
	IndexBuffer& MeshParts::GetAnimatedIndexBuffer(int meshNo, int matNo)
	{
		return m_computedAnimationVertexBuffer->GetAnimatedIndexBuffer(meshNo, matNo);
	}
	void MeshParts::BindSkeleton(Skeleton& skeleton)
	{
		m_skeleton = &skeleton;
		//構造化バッファを作成する。
		m_boneMatricesStructureBuffer.Init(
			sizeof(Matrix),
			m_skeleton->GetNumBones(),
			m_skeleton->GetBoneMatricesTopAddress()
		);
	}
	void MeshParts::Draw(
		RenderContext& rc,
		const Matrix& mWorld,
		const Matrix& mView,
		const Matrix& mProj,
		int numInstance
	)
	{
		//メッシュごとにドロー
		//プリミティブのトポロジーはトライアングルリストのみ。
		rc.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//定数バッファを更新する。
		SConstantBuffer cb;
		cb.mWorld = mWorld;
		cb.mView = mView;
		cb.mProj = mProj;
		m_commonConstantBuffer.CopyToVRAM(cb);

		if (m_expandData) {
			m_expandConstantBuffer.CopyToVRAM(m_expandData);
		}
		if (m_boneMatricesStructureBuffer.IsInited()) {
			//ボーン行列を更新する。
			m_boneMatricesStructureBuffer.Update(m_skeleton->GetBoneMatricesTopAddress());
		}
		int descriptorHeapNo = 0;
		int meshNo = 0;
		for (auto& mesh : m_meshs) {
			if (m_computedAnimationVertexBuffer){
				// 頂点バッファの事前計算処理が指定されているので、計算済み頂点バッファを取得する。
				rc.SetVertexBuffer(m_computedAnimationVertexBuffer->GetAnimatedVertexBuffer(meshNo));
			}
			else {
				rc.SetVertexBuffer(mesh->m_vertexBuffer);
			}
			
			//マテリアルごとにドロー。
			for (int matNo = 0; matNo < mesh->m_materials.size(); matNo++) {
				//このマテリアルが貼られているメッシュの描画開始。
				mesh->m_materials[matNo]->BeginRender(
					rc, 
					mesh->skinFlags[matNo]
				);
				//2. ディスクリプタヒープを設定。
				rc.SetDescriptorHeap(m_descriptorHeap);
				//3. インデックスバッファを設定。
				IndexBuffer* ib = nullptr;
				if (m_computedAnimationVertexBuffer) {
					ib = &m_computedAnimationVertexBuffer->GetAnimatedIndexBuffer(meshNo, matNo);
				}
				else {
					ib = mesh->m_indexBufferArray[matNo];
				}
				rc.SetIndexBuffer(*ib);
				//4. ドローコールを実行。
				rc.DrawIndexedInstance(ib->GetCount(), numInstance);
				descriptorHeapNo++;
			}
			meshNo++;
		}
	}
}