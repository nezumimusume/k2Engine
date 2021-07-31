/*!
 * @brief	コライダー。
 */

#pragma once

class btCollisionShape;
/// <summary>
/// コライダーのインターフェースクラス。
/// </summary>
class ICollider : public Noncopyable{
public:
	virtual btCollisionShape* GetBody() const = 0;
};
