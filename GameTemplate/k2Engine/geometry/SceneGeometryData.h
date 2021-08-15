#pragma once

#include "geometry/GeometryData.h"

namespace nsK2Engine {
    /// <summary>
    /// �V�[���̃W�I���g�����
    /// </summary>
    class SceneGeometryData : public Noncopyable {
    public:
        /// <summary>
        /// �􉽊w�f�[�^��o�^
        /// </summary>
        /// <param name="geomData">�􉽊w�f�[�^</param>
        void RegisterGeometryData(GemometryData* geomData)
        {
            m_geometryDatas.push_back(geomData);
        }
        /// <summary>
        /// �􉽊w�f�[�^�̓o�^�����B
        /// </summary>
        /// <param name="geomData"></param>
        void UnregisterGeometryData(GemometryData* geomData)
        {
            auto it = std::find(
                m_geometryDatas.begin(),
                m_geometryDatas.end(),
                geomData
            );
            if (it != m_geometryDatas.end()) {
                m_geometryDatas.erase(it);
            }
        }
        /// <summary>
        /// �􉽊w�����X�V�B
        /// </summary>
        void Update();
        const Vector3& GetShadowCasterMaxPositionInViewFrustum() const
        {
            return m_shadowCasterMaxPositionInViewFrustum;
        }
        const Vector3& GetShadowCasterMinPositionInViewFrustum() const
        {
            return m_shadowCasterMinPositionInViewFrustum;
        }
        bool IsBuildshadowCasterGeometryData() const
        {
            return m_isBuildshadowCasterGeometryData;
        }
    private:
        Vector3 m_shadowCasterMaxPositionInViewFrustum;     // �r���[�t���X�^���Ɋ܂܂�Ă���V���h�E�L���X�^�[�̍ő���W�B
        Vector3 m_shadowCasterMinPositionInViewFrustum;     // �r���[�t���X�^���Ɋ܂܂�Ă���V���h�E�L���X�^�[�̍ŏ����W�B
        bool m_isBuildshadowCasterGeometryData = false;     // �V���h�E�L���X�^�[�Ɋւ���W�I���g�������\�z�ł����H
        std::list< GemometryData* > m_geometryDatas;        // �V�[�����\������W�I���g�����̃��X�g�B
    };
}