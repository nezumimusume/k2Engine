#include "k2EnginePreCompile.h"
#include "CascadeShadowMapMatrix.h"

namespace nsK2Engine {
    void CascadeShadowMapMatrix::CalcLightViewProjectionCropMatrix(
        Vector3 lightDirection,
        float cascadeAreaRateTbl[NUM_SHADOW_MAP],
        const Vector3& sceneMaxPosition,
        const Vector3& sceneMinPosition
    )
    {
        // 最大ファーは150m
        float maxFar = g_camera3D->GetFar() * cascadeAreaRateTbl[NUM_SHADOW_MAP-1];
        //ビュー行列を計算する。
        Matrix viewMatrix;
        Vector3 lightTarget = g_camera3D->GetPosition();
        Vector3 lightPos = lightTarget;
        // ライトの高さは50m決め打ち。
        float lightMaxHeight = 5000.0f;
        lightPos += (lightDirection) * (lightMaxHeight / lightDirection.y);
        //上方向を設定
        if (fabsf(lightDirection.y) > 0.9999f) {
            //ほぼ真上、真下を向いている
            viewMatrix.MakeLookAt(lightPos, lightTarget, g_vec3AxisX);
        }
        else {
            viewMatrix.MakeLookAt(lightPos, lightTarget, g_vec3AxisY);
        }
        Matrix projMatrix;
        projMatrix.MakeOrthoProjectionMatrix(
            5000.0f,
            5000.0f,
            1.0f,
            maxFar
        );

        // 分割エリアの最大深度値を定義する
        float cascadeAreaTbl[NUM_SHADOW_MAP] = {
            maxFar * cascadeAreaRateTbl[SHADOW_MAP_AREA_NEAR],     // 近影を映す最大深度値
            maxFar * cascadeAreaRateTbl[SHADOW_MAP_AREA_MIDDLE],   // 中影を映す最大深度値
            maxFar * cascadeAreaRateTbl[SHADOW_MAP_AREA_FAR] ,     // 遠影を映す最大深度値。
        };
        // カメラの前方向、右方向、上方向を求める
        // 前方向と右方向はすでに計算済みなので、それを引っ張ってくる
        const auto& cameraForward = g_camera3D->GetForward();
        const auto& cameraRight = g_camera3D->GetRight();

        // カメラの上方向は前方向と右方向の外積で求める
        Vector3 cameraUp;
        cameraUp.Cross(cameraForward, cameraRight);
        // nearDepthはエリアの最小深度値を表す
            // 一番近いエリアの最小深度値はカメラのニアクリップ
        float nearDepth = g_camera3D->GetNear();
        for (int areaNo = 0; areaNo < NUM_SHADOW_MAP; areaNo++)
        {
            // step-7 エリアを内包する視錐台の８頂点を求める
            // エリアの近平面の中心からの上面、下面までの距離を求める
            float nearY = tanf(g_camera3D->GetViewAngle() * 0.5f) * nearDepth;

            // エリアの近平面の中心からの右面、左面までの距離を求める
            float nearX = nearY * g_camera3D->GetAspect();

            // エリアの遠平面の中心からの上面、下面までの距離を求める
            float farY = tanf(g_camera3D->GetViewAngle() * 0.5f) * cascadeAreaTbl[areaNo];

            // エリアの遠平面の中心からの右面、左面までの距離を求める
            float farX = farY * g_camera3D->GetAspect();

            // エリアの近平面の中心座標を求める
            Vector3 nearPos = g_camera3D->GetPosition() + cameraForward * nearDepth;

            // エリアの遠平面の中心座標を求める
            Vector3 farPos = g_camera3D->GetPosition() + cameraForward * cascadeAreaTbl[areaNo];

            // 8頂点を求める
            Vector3 vertex[8];

            // 近平面の右上の頂点
            vertex[0] += nearPos + cameraUp * nearY + cameraRight * nearX;

            // 近平面の左上の頂点
            vertex[1] += nearPos + cameraUp * nearY + cameraRight * -nearX;

            // 近平面の右下の頂点
            vertex[2] += nearPos + cameraUp * -nearY + cameraRight * nearX;

            // 近平面の左下の頂点
            vertex[3] += nearPos + cameraUp * -nearY + cameraRight * -nearX;

            // 遠平面の右上の頂点
            vertex[4] += farPos + cameraUp * farY + cameraRight * farX;

            // 遠平面の左上の頂点
            vertex[5] += farPos + cameraUp * farY + cameraRight * -farX;

            // 遠平面の右下の頂点
            vertex[6] += farPos + cameraUp * -farY + cameraRight * farX;

            // 遠平面の左下の頂点
            vertex[7] += farPos + cameraUp * -farY + cameraRight * -farX;

            // 8頂点をカメラ空間に変換して、近平面と遠平面を求める。
            float nearZ = FLT_MAX, farZ = -FLT_MAX;
            for (auto v : vertex) {
                viewMatrix.Apply(v);
                nearZ = max(0.0f, min(v.z, nearZ));
                farZ = max(v.z, farZ);
            }

            // 8頂点をライトビュープロジェクション空間に変換して、8頂点の最大値、最小値を求める
            Matrix lvpMatrix = viewMatrix * projMatrix;
            Vector3 vMax, vMin;
            vMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
            vMin = { FLT_MAX,  FLT_MAX,  FLT_MAX };
            for (auto v : vertex)
            {
                lvpMatrix.Apply(v);
                vMax.Max(v);
                vMin.Min(v);
            }

            // step-9 クロップ行列を求める
            float xScale = 2.0f / (vMax.x - vMin.x);
            float yScale = 2.0f / (vMax.y - vMin.y);
            float xOffset = (vMax.x + vMin.x) * -0.5f * xScale;
            float yOffset = (vMax.y + vMin.y) * -0.5f * yScale;
            Matrix clopMatrix;
            clopMatrix.m[0][0] = xScale;
            clopMatrix.m[1][1] = yScale;
            clopMatrix.m[3][0] = xOffset;
            clopMatrix.m[3][1] = yOffset;

            // ライトビュープロジェクション行列にクロップ行列を乗算する
            m_lvpcMatrix[areaNo] = lvpMatrix * clopMatrix;

            // 次のエリアの近平面までの距離を代入する
            nearDepth = cascadeAreaTbl[areaNo];
        }
    };
}