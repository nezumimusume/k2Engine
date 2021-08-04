#pragma once

#include "geometry/AABB.h"

/// <summary>
/// �W�I���g�����
/// </summary>
class GemometryData : public Noncopyable{
private:
	AABB m_aabb;		// AABB
public:
	/// <summary>
	/// �W�I���g�������X�V
	/// </summary>
	/// <param name="worldMatrix">���[���h�s��</param>
	void Update(const Matrix& worldMatrix);
};