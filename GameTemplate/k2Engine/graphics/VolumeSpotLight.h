#pragma once

namespace nsK2Engine {
	struct SpotLight;
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
		/// 初期化。
		/// </summary>
		void Init();
		/// <summary>
		/// スポットライトの情報をもとに更新
		/// </summary>
		/// <param name="spotLight">スポットライト</param>
		void Update(const SpotLight& spotLight);
		/// <summary>
		/// ボリュームライトの背面を描画
		/// </summary>
		/// <remark>
		/// この関数はエンジン側から呼ばれます。
		/// ゲーム側から呼び出ししないように。
		/// </remark>
		/// <param name="rc">レンダリングコンテキスト</param>
		void DrawToVolumeLightMapBack(RenderContext& rc);
		/// <summary>
		/// ボリュームライトの前面を描画。
		/// </summary>
		/// <remark>
		/// この関数はエンジン側から呼ばれます。
		/// ゲーム側から呼び出ししないように。
		/// </remark>
		/// <param name="rc">レンダリングコンテキスト</param>
		void DrawToVolumeLightMapFront(RenderContext& rc);
	private:
		/// <summary>
		/// GPUに送るスポットライト情報
		/// </summary>
		struct SpotLightInfo {
			int no;	// スポットライトの番号。
		};
		SpotLightInfo m_spotLightInfo;	// GPUに送るスポットライト情報。
		Model m_modelFront;	// ボリュームライトの前面描画用のモデル。
		Model m_modelBack;	// ボリュームライトの背面描画用のモデル。
	};
}