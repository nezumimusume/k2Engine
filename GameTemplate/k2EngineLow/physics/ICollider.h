/*!
 * @brief	�R���C�_�[�B
 */

#pragma once

class btCollisionShape;
namespace nsK2EngineLow {
	/// <summary>
	/// �R���C�_�[�̃C���^�[�t�F�[�X�N���X�B
	/// </summary>
	class ICollider : public Noncopyable {
	public:
		virtual btCollisionShape* GetBody() const = 0;
	};
}
