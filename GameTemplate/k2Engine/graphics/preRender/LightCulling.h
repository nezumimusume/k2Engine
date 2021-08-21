#pragma once

namespace nsK2Engine {
	/// <summary>
	/// ライトカリング。
	/// </summary>
	class LightCulling : public Noncopyable{
	public:
		/// <summary>
		/// 初期化。
		/// </summary>
		/// <param name="depthTexture">シーンの深度値が書き込まれるテクスチャ</param>
		/// /// <param name="lightCb">ライトデータの定数バッファ。</param>
		void Init(Texture& depthTexture, ConstantBuffer& lightCB, RWStructuredBuffer& pointLightNoListInTileUAV);

		/// <summary>
		/// ライトカリングを実行。
		/// </summary>
		/// <param name="rc">レンダリングコンテキスト</param>
		void Execute(RenderContext& rc);
	private:
		// ライトカリングで使用するカメラ情報
		struct CameraData
		{
			Matrix mProj;           // プロジェクション行列
			Matrix mProjInv;        // プロジェクション行列の逆行列
			Matrix mCameraRot;      // カメラの回転行列
			Vector4 screenParam;    // スクリーン情報
		};
		RootSignature m_rootSignature;					// ルートシグネチャ。
		ConstantBuffer m_cameraDataCB;					// カメラデータの定数バッファ
		PipelineState m_pipelineState;					// パイプラインステート。
		Shader m_shader;								// ライトカリングシェーダー。
		DescriptorHeap m_descriptroHeap;				// ディスクリプタヒープ。
		RWStructuredBuffer* m_pointLightNoListInTileUAV = nullptr;	// タイルごとのポイントライトのリストのUAV。
	};
}