/*!
 * @brief	�x�N�g���N���X�B
 */

#include "Vector.h"
#include "Matrix.h"

const Vector4 Vector4::White = { 1.0f, 1.0f, 1.0f, 1.0f };
const Vector4 Vector4::Black = { 0.0f, 0.0f, 0.0f, 1.0f };
const Vector4 Vector4::Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
const Vector4 Vector4::Gray = { 0.5f, 0.5f, 0.5f, 1.0f };

const Vector2 Vector2::Zero = { 0.0f,  0.0f };
const Vector3 Vector3::Zero = { 0.0f,  0.0f,  0.0f };
const Vector3 Vector3::Right = { 1.0f,  0.0f,  0.0f };
const Vector3 Vector3::Left = { -1.0f,  0.0f,  0.0f };
const Vector3 Vector3::Up = { 0.0f,  1.0f,  0.0f };
const Vector3 Vector3::Down = { 0.0f, -1.0f,  0.0f };
const Vector3 Vector3::Front = { 0.0f,   0.0f,  1.0f };
const Vector3 Vector3::Back = { 0.0f,   0.0f, -1.0f };

const Vector3 Vector3::AxisX = { 1.0f,  0.0f,  0.0f };
const Vector3 Vector3::AxisY = { 0.0f,  1.0f,  0.0f };
const Vector3 Vector3::AxisZ = { 0.0f,  0.0f,  1.0f };
const Vector3 Vector3::One = { 1.0f, 1.0f, 1.0f };
const Quaternion Quaternion::Identity = { 0.0f,  0.0f, 0.0f, 1.0f };

/*!
*@brief	�s�񂩂�N�H�[�^�j�I�����쐬�B
*/
void Quaternion::SetRotation(const Matrix& m)
{
	DirectX::XMStoreFloat4(&vec, DirectX::XMQuaternionRotationMatrix(m));
}
/*!
*@brief	from�x�N�g������to�x�N�g���ɉ�]������N�H�[�^�j�I�����쐬�B
*/
void Quaternion::SetRotation(Vector3 from, Vector3 to)
{
	from.Normalize();
	to.Normalize();
	auto t = from.Dot(to);
	Vector3 rotAxis;
	if (t > 0.998f) {
		//�قړ��������Ȃ̂ŒP�ʃN�H�[�^�j�I���ɂ���B
		*this = Quaternion::Identity;
		return;
	}
	else if (t < -0.998f) {
		//�قڋt�����Ȃ̂ŁA
		if (fabsf(to.x) < 1.0f) {
			//
			rotAxis = Cross(Vector3::AxisX, to);
		}
		else {
			rotAxis = Cross(Vector3::AxisY, to);
		}
	}
	else {
		rotAxis = Cross(from, to);
	}
	rotAxis.Normalize();
	SetRotation(rotAxis, acosf(t));
}
