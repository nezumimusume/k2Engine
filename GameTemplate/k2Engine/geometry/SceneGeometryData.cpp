#include "k2EnginePreCompile.h"
#include "geometry/SceneGeometryData.h"

namespace nsK2Engine {
    void SceneGeometryData::Update()
    {
        m_isBuildshadowCasterGeometryData = false;
        m_shadowCasterMaxPositionInViewFrustum = { -FLT_MIN, -FLT_MIN, -FLT_MIN };
        m_shadowCasterMinPositionInViewFrustum = { FLT_MAX,  FLT_MAX,  FLT_MAX };

        // �V�[���̃W�I���g�������X�V�B
        for (auto& geomData : m_geometryDatas) {
            geomData->Update();
            // �r���[�t���X�^���ɓ����Ă���V���h�E�L���X�^�[��AABB�̍ő���W�ƍŏ����W���X�V
            if (geomData->IsShadowCaster()
                && geomData->IsInViewFrustum()
                ) {
                m_shadowCasterMaxPositionInViewFrustum.Max(geomData->GetAabbMaxPositionInWorld());
                m_shadowCasterMinPositionInViewFrustum.Min(geomData->GetAabbMinPositionInWorld());
                m_isBuildshadowCasterGeometryData = true;
            }
        }
    }
}