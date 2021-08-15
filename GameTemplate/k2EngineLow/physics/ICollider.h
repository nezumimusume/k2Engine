/*!
 * @brief	コライダー。
 */

#pragma once

class btCollisionShape;
namespace nsK2EngineLow {
	/// <summary>
	/// コライダーのインターフェースクラス。
	/// </summary>
	class ICollider : public Noncopyable {
	public:
		virtual btCollisionShape* GetBody() const = 0;
	};
}
