#pragma once

namespace nsK2Engine {
	/// <summary>
	/// AABB
	/// </summary>
	class AABB {
	private:
		Vector3 m_max;					// AABB���\������{�b�N�X�̍ő���W�B
		Vector3 m_min;					// AABB���\������{�b�N�X�̍ŏ����W�B
		Vector3 m_vertexPosition[8];	// AABB���\������8���_�̍��W�B
	public:
		/// <summary>
		/// AABB�̏�������������B
		/// </summary>
		/// <param name="vMax">�ő�l</param>
		/// <param name="vMin">�ŏ��l</param>
		void Init(const Vector3& vMax, const Vector3& vMin);
		/// <summary>
		/// AABB���\������8���_�̃��[���h��Ԃł̍��W���v�Z����B
		/// </summary>
		/// <param name="pos"></param>
		/// <param name="mWorld"></param>
		void CalcVertexPositions(Vector3* pos, const Matrix& mWorld);
	};
}