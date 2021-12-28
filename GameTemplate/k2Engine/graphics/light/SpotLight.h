#pragma once

namespace nsK2Engine {
	struct SSpotLight;
	class SpotLight : Noncopyable{
	public:
		// デストラクタ。
		~SpotLight();
		/// <summary>
		/// 初期化。
		/// </summary>
		void Init();
        /// <summary>
        /// スポットライトの番号を取得。
        /// </summary>
        /// <returns></returns>
        int GetNo() const
        {
            return m_spotLight->GetNo();
        }
        /// <summary>
        /// スポットライトの番号を設定。
        /// </summary>
        /// <param name="no"></param>
        void SetNo(int no)
        {
            m_spotLight->SetNo(no);
        }
        /// <summary>
        /// 射出方向を設定。
        /// </summary>
        /// <param name="direction"></param>
        void SetDirection(const Vector3& direction)
        {
            m_spotLight->SetDirection(direction);
        }
        /// <summary>
        /// 射出方向を設定。
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="z"></param>
        void SetDirection(float x, float y, float z)
        {
            m_spotLight->SetDirection(x, y, z);
        }
        /// <summary>
        /// 射出方向を取得。
        /// </summary>
        /// <returns></returns>
        const Vector3& GetDirection() const
        {
            return m_spotLight->GetDirection();
            
        }
        /// <summary>
        /// 座標を設定。
        /// </summary>
        /// <param name="position"></param>
        void SetPosition(const Vector3& position)
        {
            m_spotLight->SetPosition(position);
        }
        void SetPosition(float x, float y, float z)
        {
            m_spotLight->SetPosition( x, y, z );
        }
        /// <summary>
        /// カラーを設定。
        /// </summary>
        /// <param name="color"></param>
        void SetColor(const Vector3& color)
        {
            m_spotLight->SetColor(color);
        }
        void SetColor(float r, float g, float b)
        {
            m_spotLight->SetColor( r, g, b );
        }
        void SetColor2(const Vector3 color)
        {
            m_spotLight->SetColor2(color);
        }
        void SetColor2(float r, float g, float b)
        {
            m_spotLight->SetColor2(r, g, b);
        }
        void SetColor3(const Vector3 color)
        {
            m_spotLight->SetColor3(color);
        }
        void SetColor3(float r, float g, float b)
        {
            m_spotLight->SetColor3(r, g, b);
        }
        /// <summary>
        /// 範囲を設定。
        /// </summary>
        /// <param name="range"></param>
        void SetRange(float range)
        {
            m_spotLight->SetRange(range);
        }

        /// <summary>
        /// 距離による影響率の累乗数を設定。
        /// </summary>
        /// <remark>
        /// SetColor()で設定した光に影響を与えます。
        /// </remark>
        /// <param name="powParam"></param>
        void SetRangeAffectPowParam(float powParam)
        {
            m_spotLight->SetRangeAffectPowParam(powParam);
        }
        void SetAngleAffectPowParam(float powParam)
        {
            m_spotLight->SetAngleAffectPowParam(powParam);
        }
        /// <summary>
        /// 距離による影響率の累乗数を設定。
        /// </summary>
        /// <remark>
        /// SetColor2()で設定した光に影響を与えます。
        /// </remark>
        /// <param name="powParam"></param>
        void SetRangeAffectPowParam2(float powParam)
        {
            m_spotLight->SetRangeAffectPowParam2(powParam);
        }
        void SetAngleAffectPowParam2(float powParam)
        {
            m_spotLight->SetAngleAffectPowParam2(powParam);
        }
        /// <summary>
        /// 距離による影響率の累乗数を設定。
        /// </summary>
        /// <remark>
        /// SetColor3()で設定した光に影響を与えます。
        /// </remark>
        /// <param name="powParam"></param>
        void SetRangeAffectPowParam3(float powParam)
        {
            m_spotLight->SetRangeAffectPowParam3(powParam);
        }
        void SetAngleAffectPowParam3(float powParam)
        {
            m_spotLight->SetAngleAffectPowParam3(powParam);
        }
        /// <summary>
        /// 離による影響率の累乗数を取得。
        /// </summary>
        /// <returns></returns>
        float GetRangeAffectPowParam() const
        {
            return m_spotLight->GetRangeAffectPowParam();
        }

        /// <summary>
        /// 射出角度
        /// </summary>
        /// <param name="angle"></param>
        void SetAngle(float angle)
        {
            m_spotLight->SetAngle(angle);
        }
        /// <summary>
        /// 二つ目の光の射出角度
        /// </summary>
        /// <param name="angle"></param>
        void SetAngle2(float angle)
        {
            m_spotLight->SetAngle2(angle);
        }
        /// <summary>
        /// 三つ目の光の射出角度
        /// </summary>
        /// <param name="angle"></param>
        void SetAngle3(float angle)
        {
            m_spotLight->SetAngle3(angle);
        }
        /// <summary>
        /// 射出角度を取得。
        /// </summary>
        /// <returns></returns>
        float GetAngle() const
        {
            return m_spotLight->GetAngle();
        }
        float GetAngle2() const
        {
            return m_spotLight->GetAngle2();
        }
        float GetAngle3() const
        {
            return m_spotLight->GetAngle3();
        }
        /// <summary>
        /// 座標を取得。
        /// </summary>
        /// <returns></returns>
        const Vector3& GetPosition() const
        {
            return m_spotLight->GetPosition();
        }
        /// <summary>
        /// カラーを取得。
        /// </summary>
        /// <returns></returns>
        const Vector3& GetColor() const
        {
            return m_spotLight->GetColor();
        }
        /// <summary>
        /// 影響範囲を取得。
        /// </summary>
        /// <returns></returns>
        float GetRange() const
        {
            return m_spotLight->GetRange();
        }
        /// <summary>
        /// 更新
        /// </summary>
        void Update()
        {
            m_spotLight->Update();
        }
        /// <summary>
        /// 生データを取得。
        /// </summary>
        /// <returns></returns>
        const SSpotLight& GetRawData() const
        {
            return *m_spotLight;
        }
	private:
		SSpotLight* m_spotLight = nullptr;
	};
}