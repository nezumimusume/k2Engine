
#pragma once

#include "math/Vector.h"

namespace nsK2EngineLow {
	/// <summary>
	/// �s��N���X�B
	/// </summary>
	class Matrix {
	public:

		union {
			DirectX::XMFLOAT4X4 mat;
			struct {
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};
			Vector4 v[4];
			float m[4][4];
		};
		//�P�ʍs��
		static const Matrix Identity;
	public:
		/// <summary>
		/// DirectX::XMMATRIX�^�ւ̈Öق̌^�ϊ��B
		/// </summary>
		/// <returns></returns>
		operator DirectX::XMMATRIX() const
		{
			return DirectX::XMLoadFloat4x4(&mat);
		}
		/// <summary>
		/// �R���X�g���N�^�B
		/// </summary>
		/// <remarks>
		/// �P�ʍs��Ƃ��ď���������܂��B
		/// </remarks>
		Matrix() {
			mat._11 = 1.0f;
			mat._12 = 0.0f;
			mat._13 = 0.0f;
			mat._14 = 0.0f;

			mat._21 = 0.0f;
			mat._22 = 1.0f;
			mat._23 = 0.0f;
			mat._24 = 0.0f;

			mat._31 = 0.0f;
			mat._32 = 0.0f;
			mat._33 = 1.0f;
			mat._34 = 0.0f;

			mat._41 = 0.0f;
			mat._42 = 0.0f;
			mat._43 = 0.0f;
			mat._44 = 1.0f;
		}
		Matrix(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33) :
			mat(m00, m01, m02, m03,
				m10, m11, m12, m13,
				m20, m21, m22, m23,
				m30, m31, m32, m33)
		{

		}
		/// <summary>
		/// �R���X�g���N�^
		/// </summary>
		/// <param name="m"></param>
		Matrix(const DirectX::XMFLOAT4X4& m)
		{
			mat = m;
		}
		/// <summary>
		/// ������Z�q�B
		/// </summary>
		/// <param name="_m"></param>
		/// <returns></returns>
		Matrix& operator=(const Matrix& _m)
		{
			mat = _m.mat;
			return *this;
		}
		/// <summary>
		/// �x�N�g����3x3�s�����Z�B
		/// </summary>
		/// <param name="vOut">��Z�����x�N�g���B</param>
		void Apply3x3(Vector3& vOut) const
		{
			Vector3 vTmp = vOut;
			vOut.x = vTmp.x * m[0][0] + vTmp.y * m[1][0] + vTmp.z * m[2][0];
			vOut.y = vTmp.x * m[0][1] + vTmp.y * m[1][1] + vTmp.z * m[2][1];
			vOut.z = vTmp.x * m[0][2] + vTmp.y * m[1][2] + vTmp.z * m[2][2];
		}

		/// <summary>
		/// �x�N�g���ɍs�����Z�B
		/// </summary>
		/// <param name="vOut">��Z�����x�N�g���B</param>
		void Apply(Vector3& vOut) const
		{
			DirectX::XMStoreFloat3(
				&vOut.vec,
				DirectX::XMVector3Transform(vOut, *this)
			);
		}
		/// <summary>
		/// �x�N�g���ɍs�����Z�B
		/// </summary>
		/// <param name="vOut">��Z�����x�N�g���B</param>
		void Apply(Vector4& vOut) const
		{
			DirectX::XMStoreFloat4(
				&vOut.vec,
				DirectX::XMVector4Transform(vOut, *this)
			);
		}
		/// <summary>
		/// ���s�ړ��s����쐬�B
		/// </summary>
		/// <param name="trans">���s�ړ��B</param>
		void MakeTranslation(const Vector3& trans)
		{
			DirectX::XMStoreFloat4x4(
				&mat,
				DirectX::XMMatrixTranslationFromVector(trans)
			);
		}
		void MakeTranslation(float x, float y, float z)
		{
			MakeTranslation(Vector3(x, y, z));
		}
		/// <summary>
		/// Y������̉�]�s����쐬�B
		/// </summary>
		/// <param name="angle">��]�p�x(�P�ʁF���W�A��)</param>
		void MakeRotationY(float angle)
		{
			DirectX::XMStoreFloat4x4(
				&mat,
				DirectX::XMMatrixRotationY(angle)
			);
		}
		/// <summary>
		/// Z������̉�]�s����쐬�B
		/// </summary>
		/// <param name="angle">��]�p�x(�P�ʁF���W�A��)</param>
		void MakeRotationZ(float angle)
		{
			DirectX::XMStoreFloat4x4(
				&mat,
				DirectX::XMMatrixRotationZ(angle)
			);
		}
		/// <summary>
		/// X������̉�]�s����쐬�B
		/// </summary>
		/// <param name="angle">��]�p�x(�P�ʁF���W�A��)</param>
		void MakeRotationX(float angle)
		{
			DirectX::XMStoreFloat4x4(
				&mat,
				DirectX::XMMatrixRotationX(angle)
			);
		}
		/// <summary>
		/// �N�H�[�^�j�I�������]�s����쐬�B
		/// </summary>
		/// <param name="q">�N�H�[�^�j�I���B</param>
		void MakeRotationFromQuaternion(const Quaternion& q)
		{
			DirectX::XMStoreFloat4x4(
				&mat,
				DirectX::XMMatrixRotationQuaternion(q)
			);
		}

		/// <summary>
		/// �C�ӂ̎�����̉�]�s����쐬�B
		/// </summary>
		/// <param name="axis">��]���B</param>
		/// <param name="angle">��]�p�x�B</param>
		void MakeRotationAxis(const Vector3& axis, float angle)
		{
			DirectX::XMStoreFloat4x4(
				&mat,
				DirectX::XMMatrixRotationAxis(axis, angle)
			);
		}
		/// <summary>
		/// �g��s����쐬�B
		/// </summary>
		/// <param name="scale">�g�嗦�B</param>
		void MakeScaling(const Vector3& scale)
		{
			DirectX::XMStoreFloat4x4(
				&mat,
				DirectX::XMMatrixScalingFromVector(scale)
			);
		}
		/// <summary>
		/// �����ϊ��s����쐬�B
		/// </summary>
		/// <param name="viewAngle">��p(�P�ʁF���W�A��)</param>
		/// <param name="aspect">�A�X�y�N�g��</param>
		/// <param name="fNear">�ߕ��ʁB</param>
		/// <param name="fFar">�����ʁB</param>
		void MakeProjectionMatrix(
			float viewAngle,
			float aspect,
			float fNear,
			float fFar
		)
		{
			DirectX::XMStoreFloat4x4(
				&mat,
				DirectX::XMMatrixPerspectiveFovLH(viewAngle, aspect, fNear, fFar)
			);
		}
		/// <summary>
		/// ���s���e�s����쐬�B
		/// </summary>
		/// <param name="w">�r���[�{�����[���̕��B</param>
		/// <param name="h">�r���[�{�����[���̍����B</param>
		/// <param name="fNear">�ߕ��ʁB</param>
		/// <param name="fFar">�����ʁB</param>
		void MakeOrthoProjectionMatrix(float w, float h, float fNear, float fFar)
		{
			DirectX::XMStoreFloat4x4(
				&mat,
				DirectX::XMMatrixOrthographicLH(w, h, fNear, fFar)
			);
		}
		/// <summary>
		/// �J�����s����쐬�B
		/// </summary>
		/// <param name="position">���_</param>
		/// <param name="target">�����_</param>
		/// <param name="up">�J�����̏����</param>
		void MakeLookAt(const Vector3& position, const Vector3& target, const Vector3& up)
		{
			DirectX::XMStoreFloat4x4(
				&mat,
				DirectX::XMMatrixLookAtLH(position, target, up)
			);
		}
		/// <summary>
		/// �s��ƍs��̏�Z
		/// </summary>
		/// <remarks>
		/// *this = m0 * m1;
		/// </remarks>
		void Multiply(const Matrix& m0, const Matrix& m1)
		{
			DirectX::XMFLOAT4X4 lm;
			DirectX::XMStoreFloat4x4(
				&lm,
				DirectX::XMMatrixMultiply(m0, m1)
			);
			mat = lm;
		}

		/// <summary>
		/// �s��̑�����Z�q
		/// </summary>
		/// <remarks>
		/// *this *= _m;
		/// return *this;
		/// </remarks>
		const Matrix& operator*=(const Matrix& _m)
		{
			Multiply(*this, _m);
			return *this;
		}
		/// <summary>
		/// �t�s����v�Z�B
		/// </summary>
		/// <param name="_m">���ɂȂ�s��B</param>
		void Inverse(const Matrix& _m)
		{
			DirectX::XMStoreFloat4x4(
				&mat,
				DirectX::XMMatrixInverse(NULL, _m)
			);
		}
		/// <summary>
		/// ���g�̋t�s����v�Z�B
		/// </summary>
		void Inverse()
		{
			Inverse(*this);
		}
		/// <summary>
		/// �]�u�s����v�Z�B
		/// </summary>
		void Transpose()
		{
			DirectX::XMStoreFloat4x4(
				&mat,
				DirectX::XMMatrixTranspose(*this)
			);
		}

	};
	/// <summary>
	/// �s�񓯎m�̏�Z
	/// </summary>
	/// <remarks>
	/// ��Z�͍�����E�Ɍ������Ă������Ă����B
	/// </remarks>
	static inline Matrix operator*(const Matrix& m1, const Matrix m2)
	{
		Matrix mRet;
		mRet.Multiply(m1, m2);
		return mRet;
	}
	/// <summary>
	/// �P�ʍs��
	/// </summary>
	const Matrix g_matIdentity = {
		1.0f, 0.0f, 0.0f, 0.0f ,
			0.0f, 1.0f, 0.0f, 0.0f ,
			0.0f, 0.0f, 1.0f, 0.0f ,
			0.0f, 0.0f, 0.0f, 1.0f
	};
}