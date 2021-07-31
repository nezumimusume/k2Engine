#pragma once

// ディレクションライト
struct DirectionalLight
{
    Vector3 direction;  // ライトの方向
    int castShadow;     // 影をキャストする？
    Vector4 color;      // ライトのカラー
};

// ライト構造体
struct Light
{
    std::array<DirectionalLight,
        NUM_DEFERRED_LIGHTING_DIRECTIONAL_LIGHT>
        directionalLight; // ディレクションライト
    Vector3 eyePos;         // カメラの位置
    float pad;
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