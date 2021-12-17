#pragma once

namespace nsK2Engine {
	struct SSpotLight;
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
		void Init(const SpotLight& spotLight);
		/// <summary>
		/// スポットライトの情報をもとに更新
		/// </summary>
		void Update();
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
		/// <summary>
		/// 最終描画
		/// </summary>
		void DrawFinal(RenderContext& rc);
	private:
		
		/// <summary>
		/// 最終描画のパスの定数バッファ。
		/// </summary>
		struct FinalCB {
			SSpotLight spotLight;   // スポットライト。
			Matrix mViewProjInv;   // ビュープロジェクション行列の逆行列
		};
		Model m_modelFront;	// ボリュームライトの前面描画用のモデル。
		Model m_modelBack;	// ボリュームライトの背面描画用のモデル。
		Sprite m_final;		// 最終描画。
		FinalCB m_finalCB;	// 最終描画の定数バッファ。
		const SpotLight* m_spotLight = nullptr;
	};
}