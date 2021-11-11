#pragma once


class Math {
public:
	//�~����
	static constexpr float V_PI = 3.14159265358979323846f;
	//�~�����̂Q��
	static constexpr float V_PI2 = V_PI * 2.0f;

	/// <summary>
	/// �p�x��Degree�P�ʂ���Radian�P�ʂɕϊ�����B
	/// </summary>
	/// <param name="deg">Degree�P�ʂ̊p�x�B</param>
	/// <returns>Radian�P�ʂ̊p�x�B</returns>
	constexpr static inline float RadFromDeg(float deg)
	{
		return deg * (V_PI / 180.0f);
	}
	/// <summary>
	/// �p�x��Radian�P�ʂ���Degree�P�ʂɕϊ�����B
	/// </summary>
	/// <param name="rad">Radian�P�ʂ̊p�x�B</param>
	/// <returns>Degree�P�ʂ̊p�x�B</returns>
	constexpr static inline float DegFromRad(float rad)
	{
		return rad / (V_PI / 180.0f);
	}
	/// <summary>
	/// t0��t1�̊Ԃ���`�⊮�B
	/// </summary>
	/// <param name="rate">��ԗ���0.0�`1.0</param>
	/// <param name="t0">��ԊJ�n�̒l�B</param>
	/// <param name="t1">��ԏI���̒l�B</param>
	/// <returns>��Ԃ��ꂽ�l�B</returns>
	template<class T> static inline T Lerp(float rate, T t0, T t1)
	{
		T ret;
		ret.Lerp(rate, t0, t1);
		return ret;
	}
	/// <summary>
	/// t0��t1�̊Ԃ���`�⊮�B
	/// </summary>
	/// <param name="rate">��ԗ���0.0�`1.0</param>
	/// <param name="t0">��ԊJ�n�̒l�B</param>
	/// <param name="t1">��ԏI���̒l�B</param>
	/// <returns>��Ԃ��ꂽ�l�B</returns>
	template<> static inline float Lerp(float rate, float t0, float t1)
	{
		return t0 + (t1 - t0) * rate;
	}
};
