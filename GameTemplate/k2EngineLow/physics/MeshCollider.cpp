#include "k2EngineLowPreCompile.h"
#include "Physics.h"
#include "MeshCollider.h"

namespace nsK2EngineLow {

	void MeshCollider::CreateFromModel(const Model& model, const Matrix& worldMatrix)
	{
		m_stridingMeshInterface = std::make_unique<btTriangleIndexVertexArray>();
		int numMesh = 0;
		const auto& tkmFile = model.GetTkmFile();
		const auto& meshParts = tkmFile.GetMeshParts();
		//メッシュを一つづつ調べていく。
		for (const auto& mesh : meshParts) {
			//まずは頂点バッファを構築する。
			VertexBufferPtr vb = std::make_unique< VertexBuffer>();
			for (const auto& vertex : mesh.vertexBuffer) {
				auto pos = vertex.pos;
				//ワールド行列を乗算する。
				worldMatrix.Apply(pos);
				vb->push_back(pos);
			}
			m_vertexBufferArray.push_back(std::move(vb));
			//続いてインデックスバッファ。
			//まずは16ビット版から。
			for (const auto& indexBuffer : mesh.indexBuffer16Array) {
				IndexBufferPtr ib = std::make_unique<IndexBuffer>();
				for (auto index : indexBuffer.indices) {
					ib->push_back(index);
				}
				m_indexBufferArray.push_back(std::move(ib));
				//インデックスメッシュを作成。
				btIndexedMesh indexedMesh;
				IndexBuffer* pIb = m_indexBufferArray.back().get();
				VertexBuffer* pVb = m_vertexBufferArray.back().get();
				indexedMesh.m_numTriangles = (int)pIb->size() / 3;
				indexedMesh.m_triangleIndexBase = (unsigned char*)(&pIb->front());
				indexedMesh.m_triangleIndexStride = 12;
				indexedMesh.m_numVertices = (int)pVb->size();
				indexedMesh.m_vertexBase = (unsigned char*)(&pVb->front());
				indexedMesh.m_vertexStride = sizeof(Vector3);
				m_stridingMeshInterface->addIndexedMesh(indexedMesh);
			}
			//次は32ビット版
			for (const auto& indexBuffer : mesh.indexBuffer32Array) {
				IndexBufferPtr ib = std::make_unique<IndexBuffer>();
				for (auto index : indexBuffer.indices) {
					ib->push_back(index);
				}
				m_indexBufferArray.push_back(std::move(ib));
				//インデックスメッシュを作成。
				btIndexedMesh indexedMesh;
				IndexBuffer* pIb = m_indexBufferArray.back().get();
				VertexBuffer* pVb = m_vertexBufferArray.back().get();
				indexedMesh.m_numTriangles = (int)pIb->size() / 3;
				indexedMesh.m_triangleIndexBase = (unsigned char*)(&pIb->front());
				indexedMesh.m_triangleIndexStride = 12;
				indexedMesh.m_numVertices = (int)pVb->size();
				indexedMesh.m_vertexBase = (unsigned char*)(&pVb->front());
				indexedMesh.m_vertexStride = sizeof(Vector3);
				m_stridingMeshInterface->addIndexedMesh(indexedMesh);
			}
		}

		m_meshShape = std::make_unique<btBvhTriangleMeshShape>(m_stridingMeshInterface.get(), true);

	}
}