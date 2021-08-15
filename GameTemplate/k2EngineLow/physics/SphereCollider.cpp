/*!
* @brief	球体コライダー。
*/

#include "k2EngineLowPreCompile.h"
#include "Physics.h"
#include "SphereCollider.h"

namespace nsK2EngineLow {
	void SphereCollider::Create(const float radius)
	{
		m_shape = std::make_unique<btSphereShape>(radius);
	}
}
