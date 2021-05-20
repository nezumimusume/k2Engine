/*!
* @brief	球体コライダー。
*/

#pragma once

#include "ICollider.h"

class SphereCollider : public ICollider
{
public:

	void Create( const float radius );
	btCollisionShape* GetBody() const override
	{
		return m_shape.get();
	}
private:
	std::unique_ptr<btSphereShape>	m_shape;
};
