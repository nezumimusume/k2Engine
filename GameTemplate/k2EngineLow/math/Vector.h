#pragma once

namespace nsK2EngineLow {

	class Matrix;
	/// <summary>
	/// 2�����x�N�g���N���X�B
	/// </summary>
	class Vector2 {
	public:
		static const Vector2 Zero;
		/// <summary>
		/// �R���X�g���N�^�B
		/// </summary>
		explicit Vector2()
		{
			x = 0.0f;
			y = 0.0f;
		}
		/// <summary>
		/// �R���X�g���N�^�B
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		Vector2(float x, float y)
		{
			this->x = x;
			this->y = y;
		}

		/// <summary>
		/// ������Z�q
		/// </summary>
		/// <param name="_v"></param>
		/// <returns></returns>
		Vector2& operator=(const Vector2& _v)
		{
			vec = _v.vec;
			return *this;
		}
		void Set(float _x, float _y)
		{
			vec.x = _x;
			vec.y = _y;
		}
		union {
			DirectX::XMFLOAT2 vec;
			struct { float x, y; };
			float v[2];
		};

		/// <summary>
		/// ���`�⊮�B
		/// </summary>
		/// <param name="t">�⊮��</param>
		/// <param name="v0">�⊮�J�n�̃x�N�g���B</param>
		/// <param name="v1">�⊮�I���̃x�N�g���B</param>
		void Lerp(float t, const Vector2& v0, const Vector2& v1)
		{
			x = v0.x + (v1.x - v0.x) * t;
			y = v0.y + (v1.y - v0.y) * t;
		}
	};
	/// <summary>
	/// 3�����x�N�g���B
	/// </summary>
	class Vector3{
	public:
		union{
			DirectX::XMFLOAT3 vec;
			float v[3];
			struct { float x, y, z; };
		};
		static const Vector3 Zero;
		static const Vector3 Right;
		static const Vector3 Left;
		static const Vector3 Up;
		static const Vector3 Down;
		static const Vector3 Front;
		static const Vector3 Back;
		static const Vector3 AxisX;
		static const Vector3 AxisY;
		static const Vector3 AxisZ;
		static const Vector3 One;
	public:
		/// <summary>
		/// XMVECTOR�ւ̈Öق̕ϊ��B
		/// </summary>
		/// <returns></returns>
		operator DirectX::XMVECTOR() const
		{
			return DirectX::XMLoadFloat3(&vec);
		}
		/// <summary>
		/// ������Z�q�B
		/// </summary>
		/// <param name="_v"></param>
		/// <returns></returns>
		Vector3& operator=(const Vector3& _v)
		{
			vec = _v.vec;
			return *this;
		}
		/// <summary>
		/// �R���X�g���N�^�B
		/// </summary>
		/// <remarks>
		/// x,y,z�S�Ă̗v�f0�ŏ���������܂��B
		/// </remarks>
		explicit Vector3()
		{
			x = y = z = 0.0f;
		}
		/// <summary>
		/// �R���X�g���N�^�B
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		Vector3(float x, float y, float z)
		{
			Set(x, y, z);
		}
		/// <summary>
		/// ���`�⊮
		/// </summary>
		/// <remarks>
		/// this = v0 + (v1-v0) * t;
		/// </remarks>
		/// <param name="t">�⊮���B</param>
		/// <param name="v0">�⊮�J�n�̃x�N�g���B</param>
		/// <param name="v1">�⊮�I���̃x�N�g���B</param>
		void Lerp(float t, const Vector3& v0, const Vector3& v1)
		{
			DirectX::XMVECTOR _v = DirectX::XMVectorLerp(
				DirectX::XMLoadFloat3(&v0.vec),
				DirectX::XMLoadFloat3(&v1.vec),
				t);
			DirectX::XMStoreFloat3(&vec, _v);
		}
		/// <summary>
		/// �x�N�g���̃R�s�[�B
		/// </summary>
		template<class TVector>
		void CopyTo(TVector& dst) const
		{
			dst.x = x;
			dst.y = y;
			dst.z = z;
		}
		/// <summary>
		/// �x�N�g���̐ݒ�B
		/// </summary>
		/// <param name="_x"></param>
		/// <param name="_y"></param>
		/// <param name="_z"></param>
		void Set(float _x, float _y, float _z)
		{
			vec.x = _x;
			vec.y = _y;
			vec.z = _z;
		}
		template<class TVector>
		void Set(TVector& _v)
		{
			Set(_v.x, _v.y, _v.z);
		}
		
		/// <summary>
		/// �x�N�g���̉��Z�B
		/// </summary>
		/// <remarks>
		/// this += _v;
		/// </remarks>
		/// <param name="_v">���Z�����x�N�g���B</param>
		void Add( const Vector3& _v) 
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&_v.vec);
			DirectX::XMVECTOR xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
			DirectX::XMStoreFloat3(&vec, xmvr);
		}
		/// <summary>
		/// �x�N�g���̉��Z�B
		/// </summary>
		/// <remarks>
		/// this = v0 + v1;
		/// </remarks>
		void Add( const Vector3& v0, const Vector3& v1 )
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&v0.vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&v1.vec);
			DirectX::XMVECTOR xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
			DirectX::XMStoreFloat3(&vec, xmvr);
		}
		/// <summary>
		/// �x�N�g���̌��Z�B
		/// </summary>
		/// <remarks>
		/// this -= _v;
		/// </remarks>
		void Subtract( const Vector3& _v )
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&_v.vec);
			DirectX::XMVECTOR xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
			DirectX::XMStoreFloat3(&vec, xmvr);
		}
		/// <summary>
		/// �x�N�g���̌��Z�B
		/// </summary>
		/// <remarks>
		/// this = v0 - v1;
		/// </remarks>
		void Subtract( const Vector3& v0, const Vector3& v1 )
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&v0.vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&v1.vec);
			DirectX::XMVECTOR xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
			DirectX::XMStoreFloat3(&vec, xmvr);
		}

		/// <summary>
		/// ���ς��v�Z�B
		/// </summary>
		/// <remarks>
		/// float d = this->x * _v.x + this->y * _v.y + this->z * _v.z;
		/// return d;
		/// </remarks>
		float Dot( const Vector3& _v ) const
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&_v.vec);
			return DirectX::XMVector3Dot(xmv0, xmv1).m128_f32[0];
		}

		/// <summary>
		/// �O�ρB
		/// </summary>
		/// <remarks>
		/// Vector3 v ;
		/// v.x = this->y * _v.z - this->z * _v.y;
		/// v.y = thiz->z * _v.x - this->x * _v.z;
		/// v.z = this->x * _v.y - this->y * _v.x;
		/// this = v;
		/// </remarks>
		void Cross(const Vector3& _v)
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&_v.vec);
			DirectX::XMVECTOR xmvr = DirectX::XMVector3Cross(xmv0, xmv1);
			DirectX::XMStoreFloat3(&vec, xmvr);
		}
		/// <summary>
		/// �O�ρB
		/// </summary>
		/// <remarks>
		/// this->x = v0.y * v1.z - v0.z * v1.y;
		/// this->y = v0.z * v1.x - v0.x * v1.z;
		/// this->z = v0.x * v1.y - v0.y * v1.x; 
		/// </remarks>
		void Cross(const Vector3& v0, const Vector3& v1)
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&v0.vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&v1.vec);
			DirectX::XMVECTOR xmvr = DirectX::XMVector3Cross(xmv0, xmv1);
			DirectX::XMStoreFloat3(&vec, xmvr);
		}
		/// <summary>
		/// �x�N�g���̒������擾
		/// </summary>
		float Length() const
		{
			DirectX::XMVECTOR xmv = DirectX::XMLoadFloat3(&vec);
			return DirectX::XMVector3Length(xmv).m128_f32[0];
		}
		/// <summary>
		/// �x�N�g���̒����̓����擾�B
		/// </summary>
		float LengthSq() const
		{
			DirectX::XMVECTOR xmv = DirectX::XMLoadFloat3(&vec);
			return DirectX::XMVector3LengthSq(xmv).m128_f32[0];
		}

		/// <summary>
		/// �x�N�g�����X�J���[�Ŋg��B
		/// </summary>
		/// <remarks>
		/// ���L�̂悤�ȏ������s���Ă��܂��B
		/// this->x *= s;
		/// this->y *= s;
		/// this->z *= s;
		/// </remarks>
		void Scale(float s)
		{
			DirectX::XMVECTOR xmv = DirectX::XMLoadFloat3(&vec);
			xmv = DirectX::XMVectorScale(xmv, s);
			DirectX::XMStoreFloat3(&vec, xmv);
		}

		/// <summary>
		/// �x�N�g���𐳋K���B
		/// </summary>
		/// <remarks>
		///		���L�̂悤�ȏ������s���Ă��܂��B
		///		//�x�N�g���̑傫�������߂�B
		///		float len = this->x * this->x + this->y * this->y + this->z * this->z;
		///		len = sqrt(len);
		///		//�x�N�g���̑傫���ŁA�e�v�f�����Z����B
		///		this->x /= len;
		///		this->y /= len;
		///		this->z /= len;
		/// </remarks>
		void Normalize()
		{
			DirectX::XMVECTOR xmv = DirectX::XMLoadFloat3(&vec);
			xmv = DirectX::XMVector3Normalize(xmv);
			DirectX::XMStoreFloat3(&vec, xmv);
		}
		/// <summary>
		/// �x�N�g�����X�J���[�ŏ��Z�B
		/// </summary>
		/// <remarks>
		///		���L�̂悤�ȏ������s���Ă��܂��B
		///		this->x /= d;
		///		this->y /= d;
		///		this->z /= d;
		/// </remarks>
		void Div(float d)
		{
			float scale = 1.0f / d;
			Scale(scale);
		}

		/// <summary>
		/// �ő�l��ݒ�B
		/// </summary>
		/// <remarks>
		/// ���L�̂悤�ȏ������s���Ă��܂��B
		/// this->x = max( this->x, v.x );
		/// this->y = max( this->y, v.y );
		/// this->z = max( this->z, v.z );
		/// </remarks>
		void Max(const Vector3& v)
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&v.vec);
			DirectX::XMStoreFloat3(&vec,  DirectX::XMVectorMax(xmv0, xmv1));
		}

		/// <summary>
		/// �ŏ��l��ݒ�B
		/// </summary>
		/// <remarks>
		/// ���L�̂悤�ȏ������s���Ă��܂��B
		/// this->x = min( this->x, v.x );
		/// this->y = min( this->y, v.y );
		/// this->z = min( this->z, v.z );
		/// </remarks>
		void Min(const Vector3& v)
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&v.vec);
			DirectX::XMStoreFloat3(&vec, DirectX::XMVectorMin(xmv0, xmv1));
		}

		/// <summary>
		/// ���Z������Z�q�B
		/// </summary>
		/// <remarks>
		///	�x�N�g���N���X�ɉ��L�̂悤�ȉ��Z����̋@�\��񋟂��܂��B
		///	Vector3 v0 = {20.0f, 30.0f, 40.0f};
		///	Vector3 v1 = {10.0f, 20.0f, 30.0f};
		///	v0 += v1;
		/// </remarks>
		const Vector3& operator+=(const Vector3& _v)
		{
			Add(_v);
			return *this;
		}
		/// <summary>
		/// ��Z������Z�q�B
		/// </summary>
		/// <remarks>
		///	�x�N�g���N���X�ɉ��L�̂悤�ȏ�Z������Z�̋@�\��񋟂��܂��B
		/// Vector3 v = {20.0f, 30.0f, 40.0f};
		///	v *= 10;
		/// </remarks>
		const Vector3& operator*=(float s) 
		{
			Scale(s);
			return *this;
		}
		/// <summary>
		/// ���Z������Z�q�B
		/// </summary>
		/// <remarks>
		/// �x�N�g���N���X�ɉ��L�̂悤�Ȍ��Z������Z�̋@�\��񋟂��܂��B
		/// Vector3 v0 = {20.0f, 30.0f, 40.0f};
		///	Vector3 v1 = {10.0f, 20.0f, 30.0f};
		///	v0 -= v1;
		/// </remarks>
		const Vector3& operator-=(const Vector3& _v)
		{
			Subtract(_v);
			return *this;
		}
		/// <summary>
		/// ���Z������Z�q�B
		/// </summary>
		/// <remarks>
		/// �x�N�g���N���X�ɉ��L�̂悤�ȏ��Z������Z�̋@�\��񋟂��܂��B
		/// Vector3 v = {20.0f, 30.0f, 40.0f};
		///	v /= 10;
		/// </remarks>
		const Vector3& operator/=(const float s)
		{
			Div(s);
			return *this;
		}
	};
	/// <summary>
	/// 4�����x�N�g���N���X�B
	/// </summary>
	class Vector4{
	public:
		union{
			DirectX::XMFLOAT4 vec;
			struct { float x, y, z, w; };
			struct { float r, g, b, a; };
			float v[4];
		};
		static const Vector4 White;
		static const Vector4 Black;
		static const Vector4 Yellow;
		static const Vector4 Gray;
	public:
		/// <summary>
		/// XMVECTOR�ւ̈Öق̌^�ϊ��B
		/// </summary>
		/// <returns></returns>
		operator DirectX::XMVECTOR() const
		{
			return DirectX::XMLoadFloat4(&vec);
		}
		/// <summary>
		/// �R���X�g���N�^�B
		/// </summary>
		/// <remarks>
		/// x,y,z��0.0�Aw��1.0�ŏ���������܂��B
		/// </remarks>
		explicit Vector4()
		{
			x = y = z = 0.0f;
			w = 1.0f;
		}
		/// <summary>
		/// �x�N�g���̃R�s�[�B
		/// </summary>
		template<class TVector4>
		void CopyTo(TVector4& dst) const
		{
			dst.x = x;
			dst.y = y;
			dst.z = z;
			dst.w = w;
		}
	
		/// <summary>
		/// ������Z�q�B
		/// </summary>
		/// <remarks>
		/// �x�N�g���N���X�ɉ��L�̂悤�ȑ�����Z�̋@�\��񋟂��܂��B
		/// Vector4 v0 = {10.0f, 20.0f, 30.0f, 10.0f};
		/// Vector4 v1;
		/// v1 = v0;
		/// </remarks>
		Vector4& operator=(const Vector4& _v)
		{
			vec = _v.vec;
			return *this;
		}

		/// <summary>
		/// �R���X�g���N�^�B
		/// </summary>
		Vector4(float x, float y, float z, float w)
		{
			Set(x, y, z, w);
		}
		/// <summary>
		/// 3�����̃x�N�g���f�[�^�������Ŏ󂯎��R���X�g���N�^
		/// </summary>
		/// <remarks>
		/// w�ɂ�1.0���i�[����܂��B
		/// </remarks>
		Vector4(const Vector3& v)
		{
			Set(v);
		}
		/// <summary>
		/// �x�N�g���̊e�v�f��ݒ�B
		/// </summary>
		void Set(float _x, float _y, float _z, float _w)
		{
			this->x = _x;
			this->y = _y;
			this->z = _z;
			this->w = _w;
		}
		/// <summary>
		/// �x�N�g���𐳋K���B
		/// </summary>
		void Normalize()
		{
			DirectX::XMVECTOR xmv = DirectX::XMLoadFloat4(&vec);
			xmv = DirectX::XMVector4Normalize(xmv);
			DirectX::XMStoreFloat4(&vec, xmv);
		}
		/// <summary>
		/// �x�N�g����ݒ�B
		/// </summary>
		/// <param name="_v"></param>
		void Set(const Vector4& _v)
		{
			*this = _v;
		}
		/// <summary>
		/// 3�����x�N�g���f�[�^�����ƂɁA�x�N�g����ݒ�B
		/// </summary>
		/// <remarks>
		/// w�ɂ�1.0���ݒ肳��܂��B
		/// </remarks>
		void Set(const Vector3& _v)
		{
			this->x = _v.x;
			this->y = _v.y;
			this->z = _v.z;
			this->w = 1.0f;
		}
		/// <summary>
		/// �x�N�g�������Z�B
		/// </summary>
		/// <remarks>
		/// this += _v;
		/// </remarks>
		void Add( const Vector4& _v )
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat4(&vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat4(&_v.vec);
			DirectX::XMVECTOR xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
			DirectX::XMStoreFloat4(&vec, xmvr);
		}
		/// <summary>
		/// �x�N�g���̉��Z�B
		/// </summary>
		/// <remarks>
		/// this = v0 + v1;
		/// </remarks>
		void Add( const Vector4& v0, const Vector4& v1 )
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat4(&v0.vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat4(&v1.vec);
			DirectX::XMVECTOR xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
			DirectX::XMStoreFloat4(&vec, xmvr);
		}
		/// <summary>
		/// �x�N�g���̌��Z�B
		/// </summary>
		/// <remarks>
		/// this -= _v;
		/// </remarks>
		void Subtract( const Vector4& _v )
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat4(&vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat4(&_v.vec);
			DirectX::XMVECTOR xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
			DirectX::XMStoreFloat4(&vec, xmvr);
		}
		/// <summary>
		/// �x�N�g���̌��Z�B
		/// </summary>
		/// <remarks>
		/// this = v0 - v1;
		/// </remarks>
		void Subtract( const Vector4& v0, const Vector4& v1 )
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat4(&v0.vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat4(&v1.vec);
			DirectX::XMVECTOR xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
			DirectX::XMStoreFloat4(&vec, xmvr);
		}

		/// <summary>
		/// ���ς��v�Z�B
		/// </summary>
		/// <remarks>
		/// float d = this->x * _v.x + this->y * _v.y + this->z * _v.z + this->w * _v.w;
		/// return d;
		/// </remarks>
		float Dot( const Vector4& _v ) const
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat4(&vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat4(&_v.vec);
			return DirectX::XMVector4Dot(xmv0, xmv1).m128_f32[0];
		}
		/// <summary>
		/// �x�N�g���̒������擾
		/// </summary>
		float Length()
		{
			DirectX::XMVECTOR xmv = DirectX::XMLoadFloat4(&vec);
			return DirectX::XMVector4Length(xmv).m128_f32[0];
		}
		/// <summary>
		/// �x�N�g���̒����̓����擾�B
		/// </summary>
		float LengthSq()
		{
			DirectX::XMVECTOR xmv = DirectX::XMLoadFloat4(&vec);
			return DirectX::XMVector4LengthSq(xmv).m128_f32[0];
		}
		/// <summary>
		/// �x�N�g�����X�J���[�Ŋg��B
		/// </summary>
		/// <remarks>
		/// ���L�̂悤�ȏ������s���Ă��܂��B
		/// this->x *= s;
		/// this->y *= s;
		/// this->z *= s;
		/// this->w *= s;
		/// </remarks>
		void Scale(float s)
		{
			DirectX::XMVECTOR xmv = DirectX::XMLoadFloat4(&vec);
			xmv = DirectX::XMVectorScale(xmv, s);
			DirectX::XMStoreFloat4(&vec, xmv);
		}
		/// <summary>
		/// �ő�l��ݒ�B
		/// </summary>
		/// <remarks>
		/// ���L�̂悤�ȏ������s���Ă��܂��B
		/// this->x = max( this->x, v.x );
		/// this->y = max( this->y, v.y );
		/// this->z = max( this->z, v.z );
		/// this->w = max( this->w, v.w );
		/// </remarks>
		void Max(const Vector4& v)
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat4(&vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat4(&v.vec);
			DirectX::XMStoreFloat4(&vec, DirectX::XMVectorMax(xmv0, xmv1));
		}

		/// <summary>
		/// �ŏ��l��ݒ�B
		/// </summary>
		/// <remarks>
		/// ���L�̂悤�ȏ������s���Ă��܂��B
		/// this->x = min( this->x, v.x );
		/// this->y = min( this->y, v.y );
		/// this->z = min( this->z, v.z );
		/// this->w = min( this->w, v.w );
		/// </remarks>
		void Min(const Vector4& v)
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat4(&vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat4(&v.vec);
			DirectX::XMStoreFloat4(&vec, DirectX::XMVectorMin(xmv0, xmv1));
		}
	};

	const Vector2 g_vec2Zero = { 0.0f, 0.0f };
	const Vector3 g_vec3Zero = { 0.0f,  0.0f,  0.0f };
	const Vector3 g_vec3Right = { 1.0f,  0.0f,  0.0f };
	const Vector3 g_vec3Left = { -1.0f,  0.0f,  0.0f };
	const Vector3 g_vec3Up = { 0.0f,  1.0f,  0.0f };
	const Vector3 g_vec3Down = { 0.0f, -1.0f,  0.0f };
	const Vector3 g_vec3Front = { 0.0f,   0.0f,  1.0f };
	const Vector3 g_vec3Back = { 0.0f,   0.0f, -1.0f };
	const Vector3 g_vec3AxisX = { 1.0f,  0.0f,  0.0f };
	const Vector3 g_vec3AxisY = { 0.0f,  1.0f,  0.0f };
	const Vector3 g_vec3AxisZ = { 0.0f,  0.0f,  1.0f };
	const Vector3 g_vec3One = { 1.0f, 1.0f, 1.0f };

	const Vector4 g_vec4White = { 1.0f, 1.0f, 1.0f, 1.0f };
	const Vector4 g_vec4Black = { 0.0f, 0.0f, 0.0f, 1.0f };
	const Vector4 g_vec4Gray = { 0.5f, 0.5f, 0.5f, 1.0f };
	const Vector4 g_vec4Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };

	/// <summary>
	/// �N�H�[�^�j�I���N���X�B
	/// </summary>
	class Quaternion : public Vector4{
	public:
		static const Quaternion Identity;		//!<�P�ʃN�H�[�^�j�I���B
		Quaternion() 
		{
			x = y = z = 0.0f;
			w = 1.0f;
		}
		/// <summary>
		/// �R���X�g���N�^�B
		/// </summary>
		Quaternion(float x, float y, float z, float w) :
			Vector4(x, y, z, w)
		{
		}
		/// <summary>
		/// X������̉�]�N�H�[�^�j�I�����쐬�B
		/// </summary>
		/// <param name="axis"></param>
		/// <param name="angle"></param>
		void SetRotationX(float angle)
		{
			SetRotation(g_vec3AxisX, angle);
		}
		/// <summary>
		/// X������̉�]�N�H�[�^�j�I�����쐬�B
		/// </summary>
		/// <param name="angle">��]�p�x�B�P��Degree</param>
		void SetRotationDegX(float angle)
		{
			SetRotationDeg(g_vec3AxisX, angle);
		}
		/// <summary>
		/// Y������̉�]�N�H�[�^�j�I�����쐬�B
		/// </summary>
		/// <param name="axis"></param>
		/// <param name="angle"></param>
		void SetRotationY(float angle)
		{
			SetRotation(g_vec3AxisY, angle);
		}
		/// <summary>
		/// Y������̉�]�N�H�[�^�j�I�����쐬�B
		/// </summary>
		/// <param name="angle">��]�p�x�B�P��Degree</param>
		void SetRotationDegY(float angle)
		{
			SetRotationDeg(g_vec3AxisY, angle);
		}
		/// <summary>
		/// Y������̉�]�N�H�[�^�j�I�����쐬�B
		/// </summary>
		/// <param name="direction">�����x�N�g���B</param>
		void SetRotationYFromDirectionXZ(const Vector3& direction)
		{
			SetRotationY(atan2f(direction.x, direction.z));
		}
		/// <summary>
		/// Z������̉�]�N�H�[�^�j�I�����쐬�B
		/// </summary>
		/// <param name="axis"></param>
		/// <param name="angle"></param>
		void SetRotationZ(float angle)
		{
			SetRotation(g_vec3AxisZ, angle);
		}
		/// <summary>
		/// Z������̉�]�N�H�[�^�j�I�����쐬�B
		/// </summary>
		/// <param name="angle">��]�p�x�B�P��Degree</param>
		void SetRotationDegZ(float angle)
		{
			SetRotationDeg(g_vec3AxisZ, angle);
		}

	

		/// <summary>
		/// �C�ӂ̎�����̉�]�N�H�[�^�j�I�����쐬�B
		/// </summary>
		/// <param name="axis">��]��</param>
		/// <param name="angle">��]�p�x�B�P�ʃ��W�A���B</param>
		void SetRotation( const Vector3& axis, float angle )
		{
			float s;
			float halfAngle = angle * 0.5f;
			s = sinf(halfAngle);
			w = cosf(halfAngle);
			x = axis.x * s;
			y = axis.y * s;
			z = axis.z * s;
		}
		/// <summary>
		/// �C�ӂ̎�����̉�]�N�H�[�^�j�I�����쐬�B
		/// </summary>
		/// <param name="axis">��]��</param>
		/// <param name="angle">��]�p�x�B�P��Degree</param>
		void SetRotationDeg(const Vector3& axis, float angle)
		{
			float s;
			float halfAngle = Math::DegToRad(angle) * 0.5f;
			s = sinf(halfAngle);
			w = cosf(halfAngle);
			x = axis.x * s;
			y = axis.y * s;
			z = axis.z * s;
		}
		/// <summary>
		/// �s�񂩂�N�H�[�^�j�I�����쐬�B
		/// </summary>
		/// <param name="m">�s��</param>
		void SetRotation(const Matrix& m);
		/// <summary>
		/// from�x�N�g������to�x�N�g���ɉ�]������N�H�[�^�j�I�����쐬�B
		/// </summary>
		/// <param name="from">��]�O�̃x�N�g��</param>
		/// <param name="to">��]��̃x�N�g��</param>
		void SetRotation( Vector3 from,  Vector3 to);
		/// <summary>
		/// ���ʐ��`�⊮
		/// </summary>
		/// <param name="t">�⊮��</param>
		/// <param name="q1">�J�n�N�H�[�^�j�I���B</param>
		/// <param name="q2">�I���N�H�[�^�j�I���B</param>
		void Slerp(float t, Quaternion q1, Quaternion q2)
		{
			DirectX::XMVECTOR xmv = DirectX::XMQuaternionSlerp(
				DirectX::XMLoadFloat4(&q1.vec),
				DirectX::XMLoadFloat4(&q2.vec),
				t
			);
			DirectX::XMStoreFloat4(&vec, xmv);
		}
		/// <summary>
		/// Y������̉�]�����Z�B
		/// </summary>
		/// <returns>���Z�����]�p�x�B���W�A���P�ʁB</returns>
		void AddRotationY(float angle)
		{
			Quaternion addRot;
			addRot.SetRotation(Vector3::AxisY, angle);
			*this *= addRot;
		}
		/// <summary>
		/// Y������̉�]�����Z�B
		/// </summary>
		/// <returns>���Z�����]�p�x�B�P��Degree</returns>
		void AddRotationDegY(float degree)
		{
			float halfAngle = Math::DegToRad(degree) * 0.5f;
			Quaternion addRot;
			addRot.SetRotation(Vector3::AxisY, halfAngle);
			*this *= addRot;
		}
		/// <summary>
		/// X������̉�]�����Z�B
		/// </summary>
		/// <returns>���Z�����]�p�x�B���W�A���P�ʁB</returns>
		void AddRotationX(float angle)
		{
			Quaternion addRot;
			addRot.SetRotation(Vector3::AxisX, angle);
			*this *= addRot;
		}
		/// <summary>
		/// X������̉�]�����Z�B
		/// </summary>
		/// <returns>���Z�����]�p�x�B�P��Degree</returns>
		void AddRotationDegX(float degree)
		{
			float halfAngle = Math::DegToRad(degree) * 0.5f;
			Quaternion addRot;
			addRot.SetRotation(Vector3::AxisX, halfAngle);
			*this *= addRot;
		}
		/// <summary>
		/// Z������̉�]�����Z�B
		/// </summary>
		/// <returns>���Z�����]�p�x�B���W�A���P�ʁB</returns>
		void AddRotationZ(float angle)
		{
			Quaternion addRot;
			addRot.SetRotation(Vector3::AxisZ, angle);
			*this *= addRot;
		}
		/// <summary>
		/// Z������̉�]�����Z�B
		/// </summary>
		/// <returns>���Z�����]�p�x�B�P��Degree</returns>
		void AddRotationDegZ(float degree)
		{
			float halfAngle = Math::DegToRad(degree) * 0.5f;
			Quaternion addRot;
			addRot.SetRotation(Vector3::AxisZ, halfAngle);
			*this *= addRot;
		}
		/// <summary>
		/// �N�H�[�^�j�I�����m�̏�Z
		/// </summary>
		/// <param name="rot"></param>
		void Multiply(const Quaternion& rot)
		{
			float pw, px, py, pz;
			float qw, qx, qy, qz;

			qw = w; qx = x; qy = y; qz = z;
			pw = rot.w; px = rot.x; py = rot.y; pz = rot.z;

			w = pw * qw - px * qx - py * qy - pz * qz;
			x = pw * qx + px * qw + py * qz - pz * qy;
			y = pw * qy - px * qz + py * qw + pz * qx;
			z = pw * qz + px * qy - py * qx + pz * qw;

		}
		/// <summary>
		/// �N�H�[�^�j�I�����m�̏�Z�B
		/// </summary>
		/// <param name="rot0"></param>
		/// <param name="rot1"></param>
		void Multiply(const Quaternion& rot0, const Quaternion& rot1)
		{
			float pw, px, py, pz;
			float qw, qx, qy, qz;

			qw = rot0.w; qx = rot0.x; qy = rot0.y; qz = rot0.z;
			pw = rot1.w; px = rot1.x; py = rot1.y; pz = rot1.z;

			w = pw * qw - px * qx - py * qy - pz * qz;
			x = pw * qx + px * qw + py * qz - pz * qy;
			y = pw * qy - px * qz + py * qw + pz * qx;
			z = pw * qz + px * qy - py * qx + pz * qw;
		}
		/// <summary>
		/// �N�H�[�^�j�I���̑����Z���Z�q
		/// </summary>
		const Quaternion& operator*=(const Quaternion& rot0)
		{
			Multiply(rot0, *this);
			return *this;
		}
		/// <summary>
		/// �x�N�g���ɃN�H�[�^�j�I����K�p����B
		/// </summary>
		void Apply(Vector4& _v) const
		{
			DirectX::XMVECTOR xmv = DirectX::XMVector3Rotate(_v, *this);
			DirectX::XMStoreFloat4(&_v.vec, xmv);
		}
		/// <summary>
		/// �x�N�g���ɃN�H�[�^�j�I����K�p����B
		/// </summary>
		void Apply(Vector3& _v) const
		{
			DirectX::XMVECTOR xmv = DirectX::XMVector3Rotate(_v, *this);
			DirectX::XMStoreFloat3(&_v.vec, xmv);
		}
	};
	
	//�����^�̃x�N�g���N���X�B
	__declspec(align(16)) class Vector4i {
	public:
		union {
			struct { int x, y, z, w; };
			int v[4];
		};
	};
	/// <summary>
	/// �x�N�g�����m�̉��Z�B
	/// </summary>
	static inline Vector3 operator+(const Vector3& v0, const Vector3& v1)
	{
		Vector3 result;
		result.Add(v0, v1);
		return result;
	}
	/// <summary>
	/// �x�N�g���ƃX�J���[�̏�Z�B
	/// </summary>

	static inline Vector3 operator*(const Vector3& v, float s)
	{
		Vector3 result;
		result = v;
		result.Scale(s);
		return result;
	}
	/// <summary>
	/// �x�N�g���ƃX�J���[�̏��Z�B
	/// </summary>
	static inline Vector3 operator/(const Vector3& v, float s)
	{
		Vector3 result;
		result = v;
		result.Div(s);
		return result;
	}
	/// <summary>
	/// �x�N�g�����m�̌��Z�B
	/// </summary>
	static inline Vector3 operator-(const Vector3& v0, const Vector3& v1)
	{
		Vector3 result;
		result.Subtract(v0, v1);
		return result;
	}

	/// <summary>
	/// �O�ς��v�Z�B
	/// </summary>
	static inline Vector3 Cross(const Vector3& v0, const Vector3& v1)
	{
		Vector3 result;
		result.Cross(v0, v1);
		return result;
	}
	/// <summary>
	/// ���ς��v�Z�B
	/// </summary>
	static inline float Dot(const Vector3& v0, const Vector3& v1)
	{
		return v0.Dot(v1);
	}

	/// <summary>
	/// �x�N�g�����m�̉��Z�B
	/// </summary>

	static inline Vector4 operator+(const Vector4& v0, const Vector4& v1)
	{
		Vector4 result;
		result.Add(v0, v1);
		return result;
	}
	/// <summary>
	/// �x�N�g���ƃX�J���[�̏�Z�B
	/// </summary>

	static inline Vector4 operator*(const Vector4& v, float s)
	{
		Vector4 result;
		result = v;
		result.Scale(s);
		return result;
	}

	/// <summary>
	/// �x�N�g�����m�̌��Z�B
	/// </summary>
	static inline Vector4 operator-(const Vector4& v0, const Vector4& v1)
	{
		Vector4 result;
		result.Subtract(v0, v1);
		return result;
	}


	/// <summary>
	/// ���ς��v�Z�B
	/// </summary>
	static inline float Dot(const Vector4& v0, const Vector4& v1)
	{
		return v0.Dot(v1);
	}


	/// <summary>
	/// �N�H�[�^�j�I�����m�̏�Z�B
	/// </summary>
	static inline Quaternion operator*(const Quaternion& q1, const Quaternion q2)
	{
		Quaternion qRet;
		qRet.Multiply(q2, q1);
		return qRet;
	}

	const Quaternion g_quatIdentity = { 0.0f,  0.0f, 0.0f, 1.0f };
}