#pragma once

#include "ICollider.h"
namespace nsK2EngineLow {
	/*!
	* @brief	�{�b�N�X�R���C�_�[�B
	*/
	class BoxCollider : public ICollider
	{
	public:

		void Create(const Vector3& size);
		btCollisionShape* GetBody() const override
		{
			return shape.get();
		}
	private:
		std::unique_ptr<btBoxShape>		shape;
	};
}