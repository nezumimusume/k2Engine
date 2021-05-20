/*!
 * @brief	コライダー。
 */

#pragma once

class btCollisionShape;
/// <summary>
/// コライダーのインターフェースクラス。
/// </summary>
class ICollider {
public:
	virtual btCollisionShape* GetBody() const = 0;
};
