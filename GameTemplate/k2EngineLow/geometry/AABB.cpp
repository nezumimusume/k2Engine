#include "k2EngineLowPreCompile.h"
#include "geometry/AABB.h"

namespace nsK2EngineLow {
	void AABB::Init(const Vector3& vMax, const Vector3& vMin)
	{
		m_max = vMax;
		m_min = vMin;
		Vector3 halfSize = (m_max - m_min) * 0.5f;
		Vector3 centerPos = (vMax + vMin) * 0.5f;
		m_centerPos = centerPos;

		m_vertexPosition[0] = centerPos;
		m_vertexPosition[0].x -= halfSize.x;
		m_vertexPosition[0].y -= halfSize.y;
		m_vertexPosition[0].z -= halfSize.z;

		m_vertexPosition[1] = centerPos;
		m_vertexPosition[1].x += halfSize.x;
		m_vertexPosition[1].y -= halfSize.y;
		m_vertexPosition[1].z -= halfSize.z;

		m_vertexPosition[2] = centerPos;
		m_vertexPosition[2].x -= halfSize.x;
		m_vertexPosition[2].y += halfSize.y;
		m_vertexPosition[2].z -= halfSize.z;

		m_vertexPosition[3] = centerPos;
		m_vertexPosition[3].x += halfSize.x;
		m_vertexPosition[3].y += halfSize.y;
		m_vertexPosition[3].z -= halfSize.z;

		m_vertexPosition[4] = centerPos;
		m_vertexPosition[4].x -= halfSize.x;
		m_vertexPosition[4].y -= halfSize.y;
		m_vertexPosition[4].z += halfSize.z;

		m_vertexPosition[5] = centerPos;
		m_vertexPosition[5].x += halfSize.x;
		m_vertexPosition[5].y -= halfSize.y;
		m_vertexPosition[5].z += halfSize.z;

		m_vertexPosition[6] = centerPos;
		m_vertexPosition[6].x -= halfSize.x;
		m_vertexPosition[6].y += halfSize.y;
		m_vertexPosition[6].z += halfSize.z;

		m_vertexPosition[7] = centerPos;
		m_vertexPosition[7].x += halfSize.x;
		m_vertexPosition[7].y += halfSize.y;
		m_vertexPosition[7].z += halfSize.z;

	}

	void AABB::CalcVertexPositions(Vector3* pos, const Matrix& mWorld)
	{
		for (int vertNo = 0; vertNo < 8; vertNo++) {
			pos[vertNo] = m_vertexPosition[vertNo];
			mWorld.Apply(pos[vertNo]);
		}
	}
}