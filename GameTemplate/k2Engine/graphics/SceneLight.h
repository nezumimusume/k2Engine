#pragma once

namespace nsK2Engine {
    // ディレクションライト
    struct DirectionalLight
    {
        Vector3 direction;  // ライトの方向
        int castShadow;     // 影をキャストする？
        Vector4 color;      // ライトのカラー
    };

    // ポイントライト。

    /// <summary>
    /// ポイントライト構造体
    /// </summary>
    /// <remark>
    /// この構造体はPOD型として扱っています。
    /// 本構造体に仮想関数などは絶対に追加しないようにしてください。
    /// memcpy、memsetなどの関数を利用している可能性があります。
    /// 仮想関数などを追加すると、仮想関数テーブルが壊されます。
    /// 
    /// また、この構造体のオブジェクトのデータはシェーダー側に定数バッファとして転送されています。
    /// メンバ変数を追加した場合は、lightCulling.fx、DeferredLighting.fxも変更する必要があります。
    /// </remark>
    struct PointLight
    {
    private:
        Vector3 position;       // 座標
        int isUse = false;      // 使用中フラグ。
        Vector3 positionInView; // カメラ空間での座標
        float pad1;
        Vector3 color;          // ライトのカラー
        float pad2;
        Vector3 attn;           // 減衰パラメータ。xに影響範囲、yには影響率に累乗するパラメータ。
        float pad3;
    public:

        /// <summary>
        /// 座標を設定。
        /// </summary>
        /// <param name="position"></param>
        void SetPosition(const Vector3& position)
        {
            this->position = position;
        }
        void SetPosition(float x, float y, float z)
        {
            SetPosition({ x, y, z });
        }
        /// <summary>
        /// カラーを設定。
        /// </summary>
        /// <param name="color"></param>
        void SetColor(const Vector3& color)
        {
            this->color = color;
        }
        void SetColor(float r, float g, float b)
        {
            SetColor({ r, g, b });
        }
        /// <summary>
        /// 範囲を設定。
        /// </summary>
        /// <param name="range"></param>
        void SetRange(float range)
        {
            attn.x = range;
        }
        /// <summary>
        /// 影響率の累乗数を設定。
        /// </summary>
        /// <param name="powParam"></param>
        void SetAffectPowParam(float powParam)
        {
            attn.y = powParam;
        }
        /// <summary>
        /// 座標を取得。
        /// </summary>
        /// <returns></returns>
        const Vector3& GetPosition() const
        {
            return position;
        }
        /// <summary>
        /// カラーを取得。
        /// </summary>
        /// <returns></returns>
        const Vector3& GetColor() const
        {
            return color;
        }
        /// <summary>
        /// 影響範囲を取得。
        /// </summary>
        /// <returns></returns>
        float GetRange() const
        {
            return attn.x;
        }
        /// <summary>
        /// ポイントライトを使用中にする。
        /// </summary>
        /// /// <remark>
        /// この関数はk2Engine内部で利用されています。
        /// ゲーム側からは使用しないように注意してください。
        /// </remark>
        void Use()
        {
            isUse = true;
        }
        /// <summary>
        /// ポイントライトを未使用にする。
        /// </summary>
        /// <remark>
        /// この関数はk2Engine内部で利用されています。
        /// ゲーム側からは使用しないように注意してください。
        /// </remark>
        void UnUse()
        {
            isUse = false;
        }
        /// <summary>
        /// 更新。
        /// </summary>
        /// <remark>
        /// この関数はk2Engine内部で利用されています。
        /// ゲーム側からは使用しないように注意してください。
        /// </remark>
        void Update();

    };
    // ライト構造体
    struct Light
    {
        DirectionalLight directionalLight[MAX_DIRECTIONAL_LIGHT];   // ディレクショナルライトの配列。
        PointLight pointLights[MAX_POINT_LIGHT];                    // ポイントライトの配列。
        Matrix mViewProjInv;    // ビュープロジェクション行列の逆行列
        Vector3 eyePos;         // カメラの位置
        int numPointLight;      // ポイントライトの数。
        Vector3 ambinetLight;   // 環境光。
    };

    /// <summary>
    /// シーンライトクラス。
    /// </summary>
    class SceneLight : public Noncopyable {
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
        /// <summary>
        /// 影をキャストする？
        /// </summary>
        /// <param name="ligNo"></param>
        /// <returns></returns>
        bool IsCastShadow(int ligNo)
        {
            return m_light.directionalLight[ligNo].castShadow;
        }
        /// <summary>
        /// シーンにポイントライトを追加
        /// </summary>

        /// <summary>
        /// シーンにポイントライトを追加
        /// </summary>
        /// <remark>
        /// 本関数を利用して追加したポイントライトは、
        /// 不要になったらDeletePointLight()を使用して、削除してください。
        /// </remark>
        /// <returns>追加されたポイントライトのアドレス</returns>
        PointLight* NewPointLight()
        {
            if (m_unusePointLightQueue.empty()) {
                // これ以上ポイントライトを追加することはできない。
                return nullptr;
            }
            // 未使用のポイントライトをでキューから取り出す。
            auto* newPt = m_unusePointLightQueue.front();
            // 使用中にする。
            newPt->Use();
            // 取り出した要素を先頭から除去。
            m_unusePointLightQueue.pop_front();
            return newPt;
        }
        /// <summary>
        /// シーンからポイントライトを削除
        /// </summary>
        /// <param name="pointLight">削除するポイントライト</param>
        void DeletePointLight(PointLight* pointLight)
        {
            if (pointLight != nullptr) {
                // フラグを未使用に変更する。
                pointLight->UnUse();
                // 未使用リストに追加する。
                m_unusePointLightQueue.push_back(pointLight);
            }
        }
        /// <summary>
        /// 環境光の計算のためのIBLテクスチャを設定。
        /// </summary>
        /// <remark>
        /// この関数を利用して、IBLテクスチャをセットすると、
        /// 環境光をIBLテクスチャからサンプリングして、それを利用した
        /// ライティングが行われます。
        /// IBLテクスチャを利用した環境光の計算をオフにしたい場合は、DisableIBLForAmbinet()を呼び出して、
        /// IBLを無効にしてください。
        /// </remark>
        /// <param name="textureFilePath">
        /// IBLテクスチャのファイルパス。
        /// キューブマップである必要があります。
        /// </param>
        /// <param name="luminance">
        /// IBLテクスチャの明るさ。
        /// <param>
        void SetIBLTextureForAmbient(const wchar_t* textureFilePath, float luminance);
        /// <summary>
        /// IBL環境光を無効にする。
        /// </summary>
        void DisableIBLTextureForAmbient()
        {
            // todo 未対応。
        }
        /// <summary>
        /// 更新
        /// </summary>
        void Update();

    private:
        Light m_light;  //シーンライト。
        std::deque< PointLight* > m_unusePointLightQueue; //未使用のポイントライトのキュー。
    };
}