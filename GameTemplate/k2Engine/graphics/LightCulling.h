#pragma once

/// <summary>
/// ライトカリング。
/// </summary>
class LightCulling {
public:
	/// <summary>
	/// 初期化。
	/// </summary>
	/// <param name="depthTexture">シーンの深度値が書き込まれるテクスチャ</param>
	/// /// <param name="lightCb">ライトデータの定数バッファ。</param>
	void Init(Texture& depthTexture, ConstantBuffer& lightCB);
private:
	// ライトカリングで使用するカメラ情報
	struct CameraData
	{
		Matrix mProj;           // プロジェクション行列
		Matrix mProjInv;        // プロジェクション行列の逆行列
		Matrix mCameraRot;      // カメラの回転行列
		Vector4 screenParam;    // スクリーン情報
	};
	RWStructuredBuffer m_pointLightNoListInTileUAV;	// タイルごとのポイントライトの番号のリスト。GPU側。
	ConstantBuffer m_cameraDataCB;					// カメラデータの定数バッファ
	
	Shader m_shader;								// ライトカリングシェーダー。
	DescriptorHeap m_descriptroHeap;				// ディスクリプタヒープ。

};