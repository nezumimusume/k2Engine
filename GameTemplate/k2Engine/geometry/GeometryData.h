#pragma once

#include "geometry/AABB.h"

namespace nsK2Engine {
	class ModelRender;

	/// <summary>
	/// �W�I���g�����
	/// </summary>
	class GemometryData {

	public:
		void Init(ModelRender* modelRender, int instanceId);
		/// <summary>
		/// �r���[�t���X�^���Ɋ܂܂�Ă���H
		/// </summary>
		/// <returns></returns>
		bool IsInViewFrustum() const
		{
			return m_isInViewFrustum;
		}
		/// <summary>
		/// ���[���h���W�n�ł�AABB�̍ő���W���擾�B
		/// </summary>
		/// <returns></returns>
		const Vector3& GetAabbMaxPositionInWorld() const
		{
			return m_aabbMax;
		}
		/// <summary>
		/// ���[���h���W�n�ł�AABB�̍ŏ����W���擾�B
		/// </summary>
		/// <returns></returns>
		const Vector3& GetAabbMinPositionInWorld() const
		{
			return m_aabbMin;
		}
		/// <summary>
		/// �V���h�E�L���X�^�[�̊􉽊w�f�[�^�H
		/// </summary>
		/// <returns></returns>
		bool IsShadowCaster() const;
		/// <summary>
		/// �W�I���g�������X�V
		/// </summary>
		void Update();
	private:
		bool m_isInViewFrustum = false;			// �r���[�t���X�^���Ɋ܂܂�Ă��邩�B
		AABB m_aabb;							// AABB
		Vector3 m_aabbWorldVertexPositions[8];	// AABB�̃��[���h��Ԃł�8���_�B
		Vector3 m_aabbMax;
		Vector3 m_aabbMin;
		ModelRender* m_modelRender = nullptr;	// ���̃W�I���g�����Ɗ֘A�Â��Ă��郂�f�������_���[�B
		int m_instanceId = 0;
	};
}