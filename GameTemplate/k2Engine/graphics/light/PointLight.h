#pragma once

namespace nsK2Engine {
	/// <summary>
	/// ポイントライト
	/// </summary>
	class PointLight : Noncopyable {
	public:
		// デストラクタ
		~PointLight();
		/// <summary>
		/// 初期化。
		/// </summary>
		void Init();
        /// <summary>
        /// 座標を設定。
        /// </summary>
        /// <param name="position"></param>
        void SetPosition(const Vector3& position)
        {
            m_pointLight->SetPosition(position);
        }
        void SetPosition(float x, float y, float z)
        {
            m_pointLight->SetPosition(x, y, z);
        }
        /// <summary>
        /// カラーを設定。
        /// </summary>
        /// <param name="color"></param>
        void SetColor(const Vector3& color)
        {
            m_pointLight->SetColor(color);
        }
        void SetColor(float r, float g, float b)
        {
            m_pointLight->SetColor( r, g, b );
        }
        /// <summary>
        /// 範囲を設定。
        /// </summary>
        /// <param name="range"></param>
        void SetRange(float range)
        {
            m_pointLight->SetRange(range);
        }
        /// <summary>
        /// 影響率の累乗数を設定。
        /// </summary>
        /// <param name="powParam"></param>
        void SetAffectPowParam(float powParam)
        {
            m_pointLight->SetAffectPowParam(powParam);
        }
        /// <summary>
        /// 座標を取得。
        /// </summary>
        /// <returns></returns>
        const Vector3& GetPosition() const
        {
            return m_pointLight->GetPosition();
        }
        /// <summary>
        /// カラーを取得。
        /// </summary>
        /// <returns></returns>
        const Vector3& GetColor() const
        {
            return m_pointLight->GetColor();
        }
        /// <summary>
        /// 影響範囲を取得。
        /// </summary>
        /// <returns></returns>
        float GetRange() const
        {
            return m_pointLight->GetRange();
        }
        /// <summary>
        /// 更新。
        /// </summary>
        void Update()
        {
            m_pointLight->Update();
        }
        /// <summary>
        /// 生データを取得。
        /// </summary>
        /// <returns></returns>
        const SPointLight& GetRawData() const
        {
            return *m_pointLight;
        }
	private:
		SPointLight* m_pointLight = nullptr;	// ポイントライト。
	};
}