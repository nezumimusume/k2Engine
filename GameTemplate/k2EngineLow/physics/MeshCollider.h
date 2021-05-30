#pragma once

#include "ICollider.h"

/// <summary>
/// メッシュコライダー
/// </summary>
class MeshCollider : public ICollider
{
public:
	/// <summary>
	/// Modelクラスからメッシュコライダーを作成。
	/// </summary>
	/// <param name="model"></param>
	/// <param name="worldMatrix"></param>
	void CreateFromModel( const Model& model, const Matrix& worldMatrix );
	/// <summary>
	/// BulletPhysicsのコライダーの形状を取得。
	/// </summary>
	/// <returns></returns>
	btCollisionShape* GetBody() const override
	{
		return m_meshShape.get();
	}
private:
	typedef std::vector<Vector3>		VertexBuffer;		//頂点バッファ。
	typedef std::vector<unsigned int>	IndexBuffer;		//インデックスバッファ。
	typedef std::unique_ptr<VertexBuffer>	VertexBufferPtr;
	typedef std::unique_ptr<IndexBuffer>	IndexBufferPtr;
	std::vector<VertexBufferPtr>					m_vertexBufferArray;		//頂点バッファの配列。
	std::vector<IndexBufferPtr>						m_indexBufferArray;		//インデックスバッファの配列。
	std::unique_ptr<btBvhTriangleMeshShape>			m_meshShape;				//メッシュ形状。
	std::unique_ptr<btTriangleIndexVertexArray>		m_stridingMeshInterface;
};

