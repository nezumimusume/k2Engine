#pragma once

#include "geometry/AABB.h"

/// <summary>
/// ジオメトリ情報
/// </summary>
class GemometryData : public Noncopyable{
private:
	AABB m_aabb;		// AABB
public:
	/// <summary>
	/// ジオメトリ情報を更新
	/// </summary>
	/// <param name="worldMatrix">ワールド行列</param>
	void Update(const Matrix& worldMatrix);
};