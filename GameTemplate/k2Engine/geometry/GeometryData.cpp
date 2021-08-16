#include "k2EnginePreCompile.h"
#include "geometry/GeometryData.h"
#include "graphics/ModelRender.h"

namespace nsK2Engine {
	void GemometryData::Update()
	{
		Matrix worldMatrix = m_modelRender->GetWorldMatrix(m_instanceId);
		m_aabb.CalcVertexPositions(
			m_aabbWorldVertexPositions,
			worldMatrix
		);

		Vector4 vMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
		Vector4 vMin = { FLT_MAX,  FLT_MAX,  FLT_MAX, FLT_MAX };

		const auto& viewProjMatrix = g_renderingEngine->GetViewProjectionMatrixForViewCulling();

		m_aabbMax = { -FLT_MIN, -FLT_MIN, -FLT_MIN };
		m_aabbMin = { FLT_MAX,  FLT_MAX,  FLT_MAX };

		m_isInViewFrustum = false;
		for (Vector4 v : m_aabbWorldVertexPositions) {

			m_aabbMax.Max({ v.x, v.y, v.z });
			m_aabbMin.Min({ v.x, v.y, v.z });

			viewProjMatrix.Apply(v);
			v.x /= fabsf(v.w);
			v.y /= fabsf(v.w);
			v.z /= fabsf(v.w);

			vMax.Max(v);
			vMin.Min(v);
		}

		if (vMax.x > -1.0f
			&& vMax.y > -1.0f
			&& vMax.z > 0.0f
			&& vMin.x < 1.0f
			&& vMin.y < 1.0f
			&& vMin.z < 1.0f
			) {
			//画面に映る。
			m_isInViewFrustum = true;
		}
	}
	bool GemometryData::IsShadowCaster() const
	{
		return m_modelRender->IsShadowCaster();
	}
	void GemometryData::Init(ModelRender* modelRender, int instanceId)
	{
		m_modelRender = modelRender;
		m_instanceId = instanceId;
		// tkmファイルからモデルの最小座標と最大座標を調べる。
		Vector3 vMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
		Vector3 vMin = { FLT_MAX,  FLT_MAX,  FLT_MAX };

		const auto& tkmFile = modelRender->GetModel().GetTkmFile();
		tkmFile.QueryMeshParts([&](const TkmFile::SMesh& mesh) {
			for (const auto& vertex : mesh.vertexBuffer) {
				vMax.Max(vertex.pos);
				vMin.Min(vertex.pos);
			}
		});
		m_aabb.Init(vMax, vMin);
	}
}