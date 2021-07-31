#pragma once

// ディレクションライト
struct DirectionalLight
{
    Vector3 direction;  // ライトの方向
    int castShadow;     // 影をキャストする？
    Vector4 color;      // ライトのカラー
};

// ポイントライト。
struct PointLight
{
    Vector3 position;       // 座標
    float pad0;
    Vector3 positionInView; // カメラ空間での座標
    float pad1;
    Vector3 color;          // ライトのカラー
    float range;            // ライトの影響を与える範囲
};
// ライト構造体
struct Light
{
    DirectionalLight directionalLight[MAX_DIRECTIONAL_LIGHT];   // ディレクショナルライトの配列。
    PointLight pointLights[MAX_POINT_LIGHT];                    // ポイントライトの配列。
    Matrix mViewProjInv;    // ビュープロジェクション行列の逆行列
    Vector3 eyePos;         // カメラの位置
    int numPointLight;      // ポイントライトの数。
    Vector3 ambinetLight;   // 環境光
};

/// <summary>
/// シーンライトクラス。
/// </summary>
class SceneLight : public Noncopyable{
public:
    /// <summary>
    /// 初期化。
    /// </summary>
    void Init();
    /// <summary>
    /// シーンライトを取得。
    /// </summary>
    /// <returns></returns>
    Light& GetSceneLight()
    {
        return m_light;
    }
    /// <summary>
    /// ディレクションライトのパラメータを設定
    /// </summary>
    /// <param name="lightNo"></param>
    /// <param name="direction"></param>
    /// <param name="color"></param>
    void SetDirectionLight(int lightNo, Vector3 direction, Vector3 color)
    {
        m_light.directionalLight[lightNo].direction = direction;
        m_light.directionalLight[lightNo].color = color;
    }
private:
    Light m_light;  //シーンライト。
};