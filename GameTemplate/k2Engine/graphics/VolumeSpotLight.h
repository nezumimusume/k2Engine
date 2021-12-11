#pragma once

namespace nsK2Engine {
	/// <summary>
	/// モデルベースのボリュームライト
	/// </summary>
	class VolumeSpotLight {
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		VolumeSpotLight();
		/// <summary>
		/// デストラクタ
		/// </summary>
		~VolumeSpotLight();
		/// <summary>
		/// ボリュームライトマップに描画
		/// </summary>
		/// <remark>
		/// この関数はエンジン側から呼ばれます。
		/// ゲーム側から呼び出ししないように。
		/// </remark>
		/// <param name="rc">レンダリングコンテキスト</param>
		void DrawToVolumeLightMap(RenderContext& rc);
	private:
		Model m_model;	// ボリュームライト描画用のモデル。

	};
}