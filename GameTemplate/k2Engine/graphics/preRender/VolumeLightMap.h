#pragma once


namespace nsK2Engine {
	/// <summary>
	/// ボリュームライトマップ。
	/// </summary>
	class VolumeLightMap : Noncopyable {
	public:
		/// <summary>
		/// 初期化。
		/// </summary>
		void Init();

		/// <summary>
		/// ボリュームライトマップを描画する。
		/// </summary>
		/// <param name="rc"></param>
		void Render(RenderContext& rc);
		/// <summary>
		/// ボリュームライトモデルの背面のZ値を書き込んだテクスチャを取得。
		/// </summary>
		/// <returns></returns>
		Texture& GetVolumeLightMapBackTexture()
		{
			return m_volumeLightMapBack.GetRenderTargetTexture();
		}
		/// <summary>
		/// ボリュームライトモデルの前面のZ値を書き込んだテクスチャを取得。
		/// </summary>
		/// <returns></returns>
		Texture& GetVolumeLightMapFrontTexture()
		{
			return m_volumeLightMapFront.GetRenderTargetTexture();
		}
		/// <summary>
		/// ボリュームスポットライトをシーンに追加
		/// </summary>
		/// <param name="lig">ライト</param>
		void AddVolumeSpotLight(VolumeSpotLight& lig)
		{
			m_volumeSpotLightArray.emplace_back(&lig);
		}
		/// <summary>
		/// ボリュームスポットライトをシーンから削除
		/// </summary>
		/// <param name="lig"></param>
		void RemoveVolumeSpotLight(VolumeSpotLight& lig)
		{
			auto it = std::find(m_volumeSpotLightArray.begin(), m_volumeSpotLightArray.end(), &lig);
			if (it != m_volumeSpotLightArray.end()) {
				m_volumeSpotLightArray.erase(it);
			}
		}
	private:
		RenderTarget m_volumeLightMapFront;                     // 手前のボリュームライトマップ。
		RenderTarget m_volumeLightMapBack;                      // 奥側のボリュームライトマップ。
		std::list< VolumeSpotLight* > m_volumeSpotLightArray;   // ボリュームスポットライトの配列。
	};
}