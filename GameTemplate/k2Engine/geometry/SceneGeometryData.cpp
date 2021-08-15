#include "k2EnginePreCompile.h"
#include "geometry/SceneGeometryData.h"

namespace nsK2Engine {
    void SceneGeometryData::Update()
    {
        m_isBuildshadowCasterGeometryData = false;
        m_shadowCasterMaxPositionInViewFrustum = { -FLT_MIN, -FLT_MIN, -FLT_MIN };
        m_shadowCasterMinPositionInViewFrustum = { FLT_MAX,  FLT_MAX,  FLT_MAX };

        // シーンのジオメトリ情報を更新。
        for (auto& geomData : m_geometryDatas) {
            geomData->Update();
            // ビューフラスタムに入っているシャドウキャスターのAABBの最大座標と最小座標を更新
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