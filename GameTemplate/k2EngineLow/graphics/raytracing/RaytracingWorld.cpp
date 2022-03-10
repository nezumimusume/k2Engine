
#include "k2EngineLowPreCompile.h"
#include "RaytracingWorld.h"
#include "graphics/Material.h"

#define align_to(_alignment, _val) (((_val + _alignment - 1) / _alignment) * _alignment)
namespace nsK2EngineLow {
	namespace raytracing {
		void AccelerationStructureBuffers::Release()
		{
			if (pScratch) {
				ReleaseD3D12Object(pScratch);
			}
			if (pResult) {
				ReleaseD3D12Object(pResult);
			}
			if (pInstanceDesc) {
				ReleaseD3D12Object(pInstanceDesc);
			}
		}
		void World::CreateRaytracingInstance(Model& model, int bufferNo)
		{
			
			int meshNo = 0;
			model.QueryMeshAndDescriptorHeap([&](const SMesh& mesh, const DescriptorHeap& ds) {
				
				for (int i = 0; i < mesh.m_materials.size(); i++) {
					const D3D12_VERTEX_BUFFER_VIEW* vertexBufferView;
					IndexBuffer* indexBuffer = nullptr;
					UINT indexCount;
					if (model.IsComputedAnimationVertexBuffer()) {
						// アニメーション済み頂点バッファの計算が行われている。
						vertexBufferView = &model.GetAnimatedVertexBuffer(meshNo).GetView();
						indexBuffer = &model.GetAnimatedIndexBuffer(meshNo, i);
					}
					else {
						vertexBufferView = &mesh.m_vertexBuffer.GetView();
						indexBuffer = mesh.m_indexBufferArray[i];

					}
					const auto& indexBufferView = indexBuffer->GetView();
					D3D12_RAYTRACING_GEOMETRY_DESC desc;
					memset(&desc, 0, sizeof(desc));
					desc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
					desc.Triangles.Transform3x4 = 0;
					desc.Triangles.VertexBuffer.StartAddress = vertexBufferView->BufferLocation;
					desc.Triangles.VertexBuffer.StrideInBytes = vertexBufferView->StrideInBytes;
					desc.Triangles.VertexCount = vertexBufferView->SizeInBytes / vertexBufferView->StrideInBytes;
					desc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
					desc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
					desc.Triangles.IndexBuffer = indexBufferView.BufferLocation;
					desc.Triangles.IndexCount = indexBufferView.SizeInBytes / 4;
					desc.Triangles.IndexFormat = indexBufferView.Format;
					InstancePtr instance = std::make_unique<Instance>();
					instance->m_originalIndexBuffer = indexBuffer;
					instance->geometoryDesc = desc;
					instance->m_material = mesh.m_materials[i];
					if (model.IsComputedAnimationVertexBuffer()) {
						instance->m_vertexBufferRWSB.Init(model.GetAnimatedVertexBuffer(meshNo), false);
						instance->m_indexBufferRWSB.Init(model.GetAnimatedIndexBuffer(meshNo, i), false);
					}
					else {
						instance->m_vertexBufferRWSB.Init(mesh.m_vertexBuffer, false);
						instance->m_indexBufferRWSB.Init(*mesh.m_indexBufferArray[i], false);
					}
					
					if (model.IsComputedAnimationVertexBuffer()	) {
						// アニメーション済み頂点バッファを利用する場合は、すでにワールド空間に変換済み。
						instance->geometoryDesc.Triangles.Transform3x4 = 0;
					}
					else {
						instance->m_worldMatrixCB.Init(sizeof(Matrix), nullptr);
						instance->geometoryDesc.Triangles.Transform3x4 = instance->m_worldMatrixCB.GetGPUVirtualAddress();
					}
					instance->m_model = &model;
					m_instances[bufferNo].emplace_back(std::move(instance));
				}
				meshNo++;
			});
		}
		void World::RegistGeometry(Model& model)
		{
			for (int bufferNo = 0; bufferNo < 2; bufferNo++) {
				// レイトレのインスタンスを作成する。
				CreateRaytracingInstance(model, bufferNo);
				
			}
		}
		void World::RemoveGeometry(Model& model)
		{
			for (auto& instance : m_instances){
				auto it = instance.begin();
				while (it != instance.end()) {
					if ((*it)->m_model == &model) {
						it = instance.erase(it);
					}
					else {
						it++;
					}
				}
			}
		}
		static bool isCommit = false;
		void World::CommitRegistGeometry(RenderContext& rc)
		{
			isCommit = true;
			// ジオメトリがコミットされたので、BLASとTLASを再構築する。
			for (int bufferNo = 0; bufferNo < 2; bufferNo++) {
				m_blasBuffer[bufferNo].Build(rc, m_instances[bufferNo], false);				
				m_topLevelASBuffers[bufferNo].Build(rc, m_instances[bufferNo], false);
			}
		}
		void World::Build(RenderContext& rc)
		{
			if (isCommit) {
				int backBufferNo = g_graphicsEngine->GetBackBufferIndex();
				// BLASを構築。
				m_blasBuffer[backBufferNo].Build(rc, m_instances[backBufferNo], true);
				// TLASを構築。
				m_topLevelASBuffers[backBufferNo].Build(rc, m_instances[backBufferNo], true);
			}
		}
	
		void World::QueryInstances(int bufferNo, std::function<void(Instance&)> queryFunc) const
		{
			for (auto& instance : m_instances[g_graphicsEngine->GetBackBufferIndex()]) {
				queryFunc(*instance);
			}
		}

	
		int World::GetNumInstance() const
		{
			int bufferNo = g_graphicsEngine->GetBackBufferIndex();
			return static_cast<int>(m_instances[bufferNo].size());
		}

	
		const BLASBuffer& World::GetBLASBuffer(int bufferNo)
		{
			int backBufferNo = bufferNo == -1 ? g_graphicsEngine->GetBackBufferIndex() : bufferNo;
			return m_blasBuffer[backBufferNo];
		}

	
		TLASBuffer& World::GetTLASBuffer(int bufferNo)
		{
			int backBufferNo = bufferNo == -1 ? g_graphicsEngine->GetBackBufferIndex() : bufferNo;
			return m_topLevelASBuffers[backBufferNo];
		}
	}//namespace raytracing
}