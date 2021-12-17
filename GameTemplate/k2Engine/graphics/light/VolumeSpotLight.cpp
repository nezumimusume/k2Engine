#include "k2EnginePreCompile.h"
#include "VolumeSpotLight.h"

namespace nsK2Engine {
	VolumeSpotLight::VolumeSpotLight()
	{
		
	}

	VolumeSpotLight::~VolumeSpotLight()
	{
		// シーンライトから削除。
		if (g_renderingEngine) {
			g_renderingEngine->RemoveVolumeSpotLight(*this);
		}
	}
	void VolumeSpotLight::Init(const SpotLight& spotLight)
	{
		m_spotLight = &spotLight;
		// シーンライトに登録。
		g_renderingEngine->AddVolumeSpotLight(*this);
		// ボリュームライトマップ描画用のモデル初期化設定を行う。
		// 背面描画、前面描画共通の初期化設定。
		ModelInitData modelInitData;
		modelInitData.m_tkmFilePath = "Assets/modelData/preset/VolumeSpotLight.tkm";
		modelInitData.m_fxFilePath = "Assets/shader/DrawVolumeLight.fx";
		modelInitData.m_expandShaderResoruceView[0] = &g_renderingEngine->GetZPrepassDepthTexture();
		// ここから前面描画用の初期化設定。
		modelInitData.m_colorBufferFormat[0] = g_drawVolumeLightMapFormat.colorBufferFormat;
		modelInitData.m_psEntryPointFunc = "PSMainFrontBack";
		modelInitData.m_cullMode = D3D12_CULL_MODE_BACK; // 前面描画なので、背面カリング。
		m_modelFront.Init(modelInitData);

		// 続いて背面描画用の初期化設定。
		// 背面描画の時にスポットライトの番号を書き込む
		modelInitData.m_colorBufferFormat[0] = g_drawVolumeLightMapFormat.colorBufferFormat;
		modelInitData.m_psEntryPointFunc = "PSMainFrontBack";
		modelInitData.m_cullMode = D3D12_CULL_MODE_FRONT; // 背面描画なので、前面カリング。
		m_modelBack.Init(modelInitData);

		// 深度値書き込み用のモデルを初期化。
		modelInitData.m_colorBufferFormat[0] = g_mainRenderTargetFormat.colorBufferFormat;
		modelInitData.m_psEntryPointFunc = "PSDepth";
		modelInitData.m_cullMode = D3D12_CULL_MODE_BACK;
		modelInitData.m_alphaBlendMode = AlphaBlendMode_Trans;
		m_drawDepthModel.Init(modelInitData);

		// 最後に最終描画
		SpriteInitData spriteInitData;
		spriteInitData.m_fxFilePath = "Assets/shader/DrawVolumeLight.fx";
		spriteInitData.m_vsEntryPointFunc = "VSFinal";
		spriteInitData.m_psEntryPoinFunc = "PSFinal";
		spriteInitData.m_colorBufferFormat[0] = g_mainRenderTargetFormat.colorBufferFormat;
		spriteInitData.m_textures[0] = &g_renderingEngine->GetGBufferAlbedoTexture();
		spriteInitData.m_textures[1] = &g_renderingEngine->GetVolumeLightRender().GetVolumeLightMapFrontTexture();
		spriteInitData.m_textures[2] = &g_renderingEngine->GetVolumeLightRender().GetVolumeLightMapBackTexture();
		spriteInitData.m_expandConstantBuffer = &m_finalCB;
		spriteInitData.m_expandConstantBufferSize = sizeof(m_finalCB);
		
		spriteInitData.m_width = FRAME_BUFFER_W;
		spriteInitData.m_height = FRAME_BUFFER_H;
		spriteInitData.m_alphaBlendMode = AlphaBlendMode_Add;
		m_final.Init(spriteInitData);

		

		
	}
	void VolumeSpotLight::DrawToVolumeLightMapBack(RenderContext& rc)
	{
		// スポットライトの番号を更新する。
		m_modelBack.Draw(rc);
	}
	void VolumeSpotLight::DrawToVolumeLightMapFront(RenderContext& rc)
	{
		m_modelFront.Draw(rc);
	}
	void VolumeSpotLight::DrawFinal(RenderContext& rc)
	{
		// 最終描画の定数バッファを更新する。
		m_finalCB.spotLight = m_spotLight->GetRawData();
		// カメラ関係のデータはDrawのパスでコピーしないと、Updateの順番で
		// 1フレーム前のカメラ情報になってしまう。
		m_finalCB.mViewProjInv = g_camera3D->GetViewProjectionMatrixInv();

		m_final.Draw(rc);
	}
	void VolumeSpotLight::DrawDepth(RenderContext& rc)
	{
		m_drawDepthModel.Draw(rc);
	}
	void VolumeSpotLight::Update()
	{
		// スポットライトの情報をもとにワールド行列を更新する。
		Vector3 pos = m_spotLight->GetPosition();
		Quaternion rot;
		rot.SetRotation(g_vec3Back, m_spotLight->GetDirection());
		
		Vector3 scale;
		scale.z = m_spotLight->GetRange();
		float angle = m_spotLight->GetAngle();
		// 角度からxyの拡大率を計算する。
		float xyScale = tan(angle) * m_spotLight->GetRange() ;
		scale.x = xyScale;
		scale.y = xyScale;
		m_modelBack.UpdateWorldMatrix(pos, rot, scale);
		m_modelFront.UpdateWorldMatrix(pos, rot, scale);
		m_drawDepthModel.UpdateWorldMatrix(pos, rot, scale);
	}
}