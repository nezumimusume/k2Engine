#pragma once

namespace nsK2Engine {

    class VolumeSpotLight;
    // ディレクションライト
    struct DirectionalLight
    {
        Vector3 direction;  // ライトの方向
        int castShadow;     // 影をキャストする？
        Vector4 color;      // ライトのカラー
    };


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
    struct SPointLight
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
    /// <summary>
    /// スポットライト構造体
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
   struct SSpotLight {
   private:
        Vector3 position;                   // 座標
        int isUse = false;                  // 使用中フラグ。
        Vector3 positionInView;             // カメラ空間での座標。
        int no = 0;                         // ライトの番号。
        Vector3 direction;                  // 射出方向。
        float range;                        // 影響範囲。
        Vector3 color = g_vec3Zero;         // ライトのカラー。
        float angleAffectMul = 1.0f;        // 角度による減衰率に乗算するパラメータ。
        Vector3 color2 = g_vec3Zero;        // 二つ目のカラー。
        float pad1;
        Vector3 color3 = g_vec3Zero;        // 三つ目のカラー。
        float pad2;
        Vector3 directionInView;            // カメラ空間での射出方向。
        float pad3;
        Vector3 rangePow = g_vec3One;       // 距離による光の影響率に累乗するパラメーター。1.0で線形の変化をする。
                                            // xが一つ目のカラー、yが二つ目のカラー、zが三つ目のカラー。
        float pad4;
        Vector3 angle;                      // 射出角度(単位：ラジアン。xが一つ目のカラー、yが二つ目のカラー、zが三つ目のカラー)。
        float pad5;
        Vector3 anglePow = g_vec3One;       // スポットライトとの角度による光の影響率に累乗するパラメータ。1.0で線形に変化する。
                                            // xが一つ目のカラー、yが二つ目のカラー、zが三つ目のカラー。
        float pad6;
   public:
       /// <summary>
       /// スポットライトの番号を取得。
       /// </summary>
       /// <returns></returns>
       int GetNo() const
       {
           return no;
       }
       /// <summary>
       /// スポットライトの番号を設定。
       /// </summary>
       /// <param name="no"></param>
       void SetNo(int no)
       {
           this->no = no;
       }
        /// <summary>
        /// 射出方向を設定。
        /// </summary>
        /// <param name="direction"></param>
        void SetDirection(const Vector3& direction)
        {
            this->direction = direction;
            this->direction.Normalize();
        }
        /// <summary>
        /// 射出方向を設定。
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="z"></param>
        void SetDirection(float x, float y, float z)
        {
            direction.Set(x, y, z);
            direction.Normalize();
        }
        /// <summary>
        /// 射出方向を取得。
        /// </summary>
        /// <returns></returns>
        const Vector3& GetDirection() const
        {
            return direction;
;        }
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
        /// 一つ目の光のカラーを設定。
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
        /// 二つ目の光のカラーを設定。
        /// </summary>
        /// <param name="color"></param>
        void SetColor2(const Vector3 color)
        {
            color2 = color;
        }
        void SetColor2(float r, float g, float b)
        {
            SetColor2({ r, g, b });
        }
        /// <summary>
        /// 三つ目の光のカラーを設定。
        /// </summary>
        /// <param name="color"></param>
        void SetColor3(const Vector3 color)
        {
            color3 = color;
        }
        void SetColor3(float r, float g, float b)
        {
            SetColor3({ r, g, b });
        }
        /// <summary>
        /// スポットライトの影響範囲を設定。
        /// </summary>
        /// <param name="range"></param>
        void SetRange(float range)
        {
            this->range = range;
        }
        /// <summary>
        /// 一つ目の光の角度による影響率に累乗する値を設定。
        /// </summary>
        /// <param name="powParam"></param>
        void SetAngleAffectPowParam(float powParam)
        {
            this->anglePow.x = powParam;
        }
        /// <summary>
        /// 二つ目の光の角度による影響率に累乗する値を設定。
        /// </summary>
        /// <param name="powParam"></param>
        void SetAngleAffectPowParam2(float powParam)
        {
            anglePow.y = powParam;
        }
        /// <summary>
        /// 三つ目の光の角度による影響率に累乗する値を設定。
        /// </summary>
        /// <param name="powParam"></param>
        void SetAngleAffectPowParam3(float powParam)
        {
            anglePow.z = powParam;
        }
        
        /// <summary>
        /// 一つ目の光の距離による影響率に累乗する値を設定。
        /// </summary>
        /// <param name="powParam"></param>
        void SetRangeAffectPowParam(float powParam)
        {
            rangePow.x = powParam;
        }
        /// <summary>
        /// 二つ目の光の距離による影響率に累乗する値を設定。
        /// </summary>
        /// <param name="powParam"></param>
        void SetRangeAffectPowParam2(float powParam)
        {
            rangePow.y = powParam;
        }
        /// <summary>
        /// 三つ目の光の距離による影響率に累乗する値を設定。
        /// </summary>
        /// <param name="powParam"></param>
        void SetRangeAffectPowParam3(float powParam)
        {
            rangePow.z = powParam;
        }
        
        /// <summary>
        /// 一つ目の光の射出角度を設定。
        /// </summary>
        /// <param name="angle"></param>
        void SetAngle(float angle)
        {
            this->angle.x = angle;
        }
        /// <summary>
        /// 二つ目の光の射出角度を設定。
        /// </summary>
        /// <param name="angle"></param>
        void SetAngle2(float angle)
        {
            this->angle.y = angle;
        }
        /// <summary>
        /// 三つ目の光の射出角度を設定。
        /// </summary>
        /// <param name="angle"></param>
        void SetAngle3(float angle)
        {
            this->angle.z = angle;
        }
        /// <summary>
        /// 一つ目の光の射出角度を取得。
        /// </summary>
        /// <returns></returns>
        float GetAngle() const
        {
            return angle.x;
        }
        /// <summary>
        /// 二つ目の光の射出角度を取得。
        /// </summary>
        /// <returns></returns>
        float GetAngle2() const
        {
            return angle.y;
        }
        /// <summary>
        /// 三つ目の光の射出角度を取得。
        /// </summary>
        /// <returns></returns>
        float GetAngle3() const
        {
            return angle.z;
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
        /// 影響範囲を取得。
        /// </summary>
        /// <returns></returns>
        float GetRange() const
        {
            return range;
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
        SPointLight pointLights[MAX_POINT_LIGHT];                    // ポイントライトの配列。
        SSpotLight spotLights[MAX_SPOT_LIGHT];                       // スポットライトの配列。
        Matrix mViewProjInv;    // ビュープロジェクション行列の逆行列
        Vector3 eyePos;         // カメラの位置
        int numPointLight;      // ポイントライトの数。
        Vector3 ambinetLight;   // 環境光。
        int numSpotLight;       // スポットライトの数。
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
        /// <remark>
        /// 本関数を利用して追加したポイントライトは、
        /// 不要になったらDeletePointLight()を使用して、削除してください。
        /// </remark>
        /// <returns>追加されたポイントライトのアドレス</returns>
        SPointLight* NewPointLight()
        {
            return NewDynamicLight<SPointLight>(m_unusePointLightQueue);
        }
        /// <summary>
        /// シーンからポイントライトを削除
        /// </summary>
        /// <param name="pointLight">削除するポイントライト</param>
        void DeletePointLight(SPointLight* pointLight)
        {
            DeleteDynamicLight<SPointLight>(pointLight, m_unusePointLightQueue);
        }
        /// <summary>
        /// シーンにスポットライトを追加
        /// </summary>
        /// <remark>
        /// この関数はエンジン側で利用されます。
        /// ゲーム側では利用しないでください。
        /// </remark>
        /// <returns>追加されたスポットライトのアドレス</returns>
        SSpotLight* NewSpotLight()
        {
            return NewDynamicLight<SSpotLight>(m_unuseSpotLightQueue);
        }
        /// <summary>
        /// シーンからスポットライトを削除
        /// </summary>
        /// <param name="spotLight">削除するスポットライト</param>
        void DeleteSpotLight(SSpotLight* spotLight)
        {
            DeleteDynamicLight<SSpotLight>(spotLight, m_unuseSpotLightQueue);
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
        /// </param>
        void SetAmbientByIBLTexture(const wchar_t* textureFilePath, float luminance);
        /// <summary>
        /// IBL環境光を無効にする。
        /// </summary>
        void DisableIBLTextureForAmbient()
        {
            // todo 未対応。
        }
        /// <summary>
        /// 環境光を設定。
        /// </summary>
        /// <param name="ambient"></param>
        void SetAmbinet(Vector3 ambient)
        {
            m_light.ambinetLight = ambient;
        }
        /// <summary>
        /// 更新
        /// </summary>
        void Update();
    private:
        /// <summary>
        /// 新しい動的ライトを追加。
        /// </summary>
        /// <typeparam name="TDynamicLight"></typeparam>
        /// <typeparam name="TQue"></typeparam>
        /// <param name="que"></param>
        /// <returns></returns>
        template<class TDynamicLight>
        TDynamicLight* NewDynamicLight(std::deque< TDynamicLight*>& que)
        {
            if (que.empty()) {
                // これ以上ライトを追加することはできない。
                return nullptr;
            }
            // 未使用のライトをでキューから取り出す。
            TDynamicLight* newPt = que.front();
            // 使用中にする。
            newPt->Use();
            // 取り出した要素を先頭から除去。
            que.pop_front();
            return newPt;
        }
        /// <summary>
        /// シーンから動的ライトを削除
        /// </summary>
        /// <param name="pointLight">削除するポイントライト</param>
        template<class TDynamicLight>
        void DeleteDynamicLight(TDynamicLight* light, std::deque< TDynamicLight*>& que)
        {
            if (light != nullptr) {
                // フラグを未使用に変更する。
                light->UnUse();
                // 未使用リストに追加する。
                que.push_back(light);
            }
        }
    private:
        Light m_light;  //シーンライト。
        std::deque< SPointLight* > m_unusePointLightQueue;       // 未使用のポイントライトのキュー。
        std::deque< SSpotLight* > m_unuseSpotLightQueue;         // 未使用のスポットライトのキュー。。
    };
}