#pragma once


namespace nsK2Engine {
	class VolumeLightBase;
	/// <summary>
	/// ボリュームライトレンダラー。
	/// </summary>
	class VolumeLightRender : Noncopyable {
	public:
		/// <summary>
		/// 初期化。
		/// </summary>
		void Init();

		/// <summary>
		/// ボリュームライトを描画する。
		/// </summary>
		void Render(RenderContext& rc, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);
		
		/// <summary>
		/// ボリュームスポットライトをレンダラーに追加
		/// </summary>
		/// <param name="lig">ライト</param>
		void AddVolumeSpotLight(VolumeLightBase& lig)
		{
			m_volumeSpotLightArray.emplace_back(&lig);
		}
		/// <summary>
		/// ボリュームスポットライトをレンダラーから削除
		/// </summary>
		/// <param name="lig"></param>
		void RemoveVolumeSpotLight(VolumeLightBase& lig)
		{
			auto it = std::find(m_volumeSpotLightArray.begin(), m_volumeSpotLightArray.end(), &lig);
			if (it != m_volumeSpotLightArray.end()) {
				m_volumeSpotLightArray.erase(it);
			}
		}
		Texture& GetVolumeLightMapFrontTexture()
		{
			return m_volumeLightMapFront.GetRenderTargetTexture();
		}
		Texture& GetVolumeLightMapBackTexture()
		{
			return m_volumeLightMapBack.GetRenderTargetTexture();
		}
	private:
		RenderTarget m_volumeLightMapFront;                     // 手前のボリュームライトマップ。
		RenderTarget m_volumeLightMapBack;                      // 奥側のボリュームライトマップ。
		std::list< VolumeLightBase* > m_volumeSpotLightArray;   // ボリュームスポットライトの配列。
	};
}