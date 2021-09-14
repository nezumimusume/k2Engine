#include "k2EnginePreCompile.h"
#include "ModelRender.h"
#include "RenderingEngine.h"

namespace nsK2Engine {
	ModelRender::ModelRender()
	{
		g_renderingEngine->AddEventListener(
			this,
			[&](RenderingEngine::EnEvent enEvent) { OnRecievedEventFromRenderingEngine(enEvent); }
		);
	}
	ModelRender::~ModelRender()
	{
		if (g_renderingEngine != nullptr) {
			g_renderingEngine->RemoveEventListener(this);
			for (auto& geomData : m_geometryDatas) {
				// レンダリングエンジンから登録解除
				g_renderingEngine->UnregisterGeometryData(&geomData);
			}
		}
	}
	void ModelRender::OnRecievedEventFromRenderingEngine(RenderingEngine::EnEvent enEvent)
	{
		if (enEvent == RenderingEngine::enEventReInitIBLTexture
			&& m_translucentModel.IsInited()
		) {
			// IBLテクスチャが更新されたので、PBRシェーダーを利用している
			// フォワードレンダリングの場合は、ディスクリプタヒープを再初期化する。
			// (IBLテクスチャを使っているので。)
			MaterialReInitData matReInitData;
			if(m_isEnableInstancingDraw) {
				// インスタンシング描画を行う場合は、拡張SRVにインスタンシング描画用のデータを設定する。
				matReInitData.m_expandShaderResoruceView[0] = &m_worldMatrixArraySB;
			}
			matReInitData.m_expandShaderResoruceView[1] = &g_renderingEngine->GetIBLTexture();
			m_translucentModel.ReInitMaterials(matReInitData);
		}
	}
	void ModelRender::SetupVertexShaderEntryPointFunc(ModelInitData& modelInitData)
	{
		if (m_isEnableInstancingDraw) {
			//インスタンシング描画。
			modelInitData.m_vsEntryPointFunc = "VSMainInstancing";
		}
		else {
			modelInitData.m_vsEntryPointFunc = "VSMain";
		}
		if (m_animationClips != nullptr) {
			if (m_isEnableInstancingDraw) {
				modelInitData.m_vsSkinEntryPointFunc = "VSMainSkinInstancing";
			}
			else {
				modelInitData.m_vsSkinEntryPointFunc = "VSMainSkin";
			}
		}
	}
	void ModelRender::IniTranslucent(
		const char* filePath,
		AnimationClip* animationClips,
		int numAnimationClips,
		EnModelUpAxis enModelUpAxis,
		bool isShadowReciever,
		int maxInstance)
	{
		//インスタンシング描画用のデータを初期化。
		InitInstancingDraw(maxInstance);
		//スケルトンを初期化。
		InitSkeleton(filePath);
		//アニメーションを初期化。
		InitAnimation(animationClips, numAnimationClips, enModelUpAxis);
		//半透明オブジェクト描画パスで使用されるモデルを初期化。
		InitModelOnTranslucent(*g_renderingEngine, filePath, enModelUpAxis, isShadowReciever);
		//ZPrepass描画用のモデルを初期化。
		InitModelOnZprepass(*g_renderingEngine, filePath, enModelUpAxis);
		//シャドウマップ描画用のモデルを初期化。
		InitModelOnShadowMap(*g_renderingEngine, filePath, enModelUpAxis);
		// 幾何学データを初期化。
		InitGeometryDatas(maxInstance);
	}

	void ModelRender::InitForwardRendering(ModelInitData& initData)
	{
		//インスタンシング描画用のデータを初期化。
		InitInstancingDraw(1);
		InitSkeleton(initData.m_tkmFilePath);

		initData.m_colorBufferFormat[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		//作成した初期化データをもとにモデルを初期化する。
		m_forwardRenderModel.Init(initData);
		//ZPrepass描画用のモデルを初期化。
		InitModelOnZprepass(*g_renderingEngine, initData.m_tkmFilePath, initData.m_modelUpAxis);
		//シャドウマップ描画用のモデルを初期化。
		InitModelOnShadowMap(*g_renderingEngine, initData.m_tkmFilePath, initData.m_modelUpAxis);
		// 幾何学データを初期化。
		InitGeometryDatas(1);
	}

	void ModelRender::Init(const char* filePath,
		AnimationClip* animationClips,
		int numAnimationClips,
		EnModelUpAxis enModelUpAxis,
		bool isShadowReciever,
		int maxInstance)
	{
		// インスタンシング描画用のデータを初期化。
		InitInstancingDraw(maxInstance);
		// スケルトンを初期化。
		InitSkeleton(filePath);
		// アニメーションを初期化。
		InitAnimation(animationClips, numAnimationClips, enModelUpAxis);
		// GBuffer描画用のモデルを初期化。
		InitModelOnRenderGBuffer(*g_renderingEngine, filePath, enModelUpAxis, isShadowReciever);
		// ZPrepass描画用のモデルを初期化。
		InitModelOnZprepass(*g_renderingEngine, filePath, enModelUpAxis);
		// シャドウマップ描画用のモデルを初期化。
		InitModelOnShadowMap(*g_renderingEngine, filePath, enModelUpAxis);
		// 幾何学データを初期化。
		InitGeometryDatas(maxInstance);

	}

	void ModelRender::InitGeometryDatas(int maxInstance)
	{
		m_geometryDatas.resize(maxInstance);
		int instanceId = 0;
		for (auto& geomData : m_geometryDatas) {
			geomData.Init(this, instanceId);
			// レンダリングエンジンに登録。
			g_renderingEngine->RegisterGeometryData(&geomData);
			instanceId++;
		}
	}
	void ModelRender::InitSkeleton(const char* filePath)
	{
		//スケルトンのデータを読み込み。
		std::string skeletonFilePath = filePath;
		int pos = (int)skeletonFilePath.find(".tkm");
		skeletonFilePath.replace(pos, 4, ".tks");
		m_skeleton.Init(skeletonFilePath.c_str());
	}

	void ModelRender::InitAnimation(AnimationClip* animationClips, int numAnimationClips, EnModelUpAxis enModelUpAxis)
	{
		m_animationClips = animationClips;
		m_numAnimationClips = numAnimationClips;
		if (m_animationClips != nullptr) {
			m_animation.Init(m_skeleton,
				m_animationClips,
				numAnimationClips);
		}
	}

	void ModelRender::InitInstancingDraw(int maxInstance)
	{
		m_maxInstance = maxInstance;
		if (m_maxInstance > 1) {
			//インスタンシング描画を行うので、
			//それ用のデータを構築する。
			//ワールド行列の配列のメモリを確保する。
			m_worldMatrixArray = std::make_unique<Matrix[]>(m_maxInstance);
			//ワールド行列をGPUに転送するためのストラクチャードバッファを確保。
			m_worldMatrixArraySB.Init(
				sizeof(Matrix),
				m_maxInstance,
				nullptr
			);
			m_isEnableInstancingDraw = true;
		}
	}

	void ModelRender::InitModelOnRenderGBuffer(
		RenderingEngine& renderingEngine,
		const char* tkmFilePath,
		EnModelUpAxis enModelUpAxis,
		bool isShadowReciever
	)
	{
		ModelInitData modelInitData;
		modelInitData.m_fxFilePath = "Assets/shader/RenderToGBufferFor3DModel.fx";

		// 頂点シェーダーのエントリーポイントをセットアップ。
		SetupVertexShaderEntryPointFunc(modelInitData);

		if (m_animationClips != nullptr) {
			//スケルトンを指定する。
			modelInitData.m_skeleton = &m_skeleton;
		}

		if (isShadowReciever) {
			modelInitData.m_psEntryPointFunc = "PSMainShadowReciever";
		}
		//モデルの上方向を指定する。
		modelInitData.m_modelUpAxis = enModelUpAxis;

		modelInitData.m_tkmFilePath = tkmFilePath;
		modelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		modelInitData.m_colorBufferFormat[1] = DXGI_FORMAT_R8G8B8A8_SNORM;
		modelInitData.m_colorBufferFormat[2] = DXGI_FORMAT_R8G8B8A8_UNORM;

		if (m_isEnableInstancingDraw) {
			// インスタンシング描画を行う場合は、拡張SRVにインスタンシング描画用のデータを設定する。
			modelInitData.m_expandShaderResoruceView[0] = &m_worldMatrixArraySB;
		}
		m_renderToGBufferModel.Init(modelInitData);

	}

	void ModelRender::InitModelOnTranslucent(
		RenderingEngine& renderingEngine,
		const char* tkmFilePath,
		EnModelUpAxis enModelUpAxis,
		bool isShadowReciever
	)
	{
		ModelInitData modelInitData;
		modelInitData.m_fxFilePath = "Assets/shader/model.fx";

		// 頂点シェーダーのエントリーポイントをセットアップ。
		SetupVertexShaderEntryPointFunc(modelInitData);

		if (m_animationClips != nullptr) {
			//スケルトンを指定する。
			modelInitData.m_skeleton = &m_skeleton;
		}

		/*if (isShadowReciever) {
			modelInitData.m_psEntryPointFunc = "PSMainShadowReciever";
		}
		*/
		if (g_renderingEngine->IsSoftShadow()) {
			modelInitData.m_psEntryPointFunc = "PSMainSoftShadow";
		}
		else {
			modelInitData.m_psEntryPointFunc = "PSMainHardShadow";
		}
		//モデルの上方向を指定する。
		modelInitData.m_modelUpAxis = enModelUpAxis;
		modelInitData.m_expandConstantBuffer = &g_renderingEngine->GetDeferredLightingCB();
		modelInitData.m_expandConstantBufferSize = sizeof(g_renderingEngine->GetDeferredLightingCB());
		modelInitData.m_tkmFilePath = tkmFilePath;
		modelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		modelInitData.m_alphaBlendMode = AlphaBlendMode_Trans;
		int expandSRVNo = 0;
		if (m_isEnableInstancingDraw) {
			// インスタンシング描画を行う場合は、拡張SRVにインスタンシング描画用のデータを設定する。
			modelInitData.m_expandShaderResoruceView[expandSRVNo] = &m_worldMatrixArraySB;
		}
		expandSRVNo++;
		modelInitData.m_expandShaderResoruceView[expandSRVNo] = &g_renderingEngine->GetIBLTexture();
		expandSRVNo++;
		g_renderingEngine->QueryShadowMapTexture([&](Texture& shadowMap) {
			modelInitData.m_expandShaderResoruceView[expandSRVNo] = &shadowMap;
			expandSRVNo++;
		});
		m_translucentModel.Init(modelInitData);
	}

	void ModelRender::InitModelOnShadowMap(
		RenderingEngine& renderingEngine,
		const char* tkmFilePath,
		EnModelUpAxis modelUpAxis
	)
	{
		ModelInitData modelInitData;
		modelInitData.m_tkmFilePath = tkmFilePath;
		modelInitData.m_modelUpAxis = modelUpAxis;
		// 頂点シェーダーのエントリーポイントをセットアップ。
		SetupVertexShaderEntryPointFunc(modelInitData);


		if (m_animationClips != nullptr) {
			//スケルトンを指定する。
			modelInitData.m_skeleton = &m_skeleton;
		}

		modelInitData.m_fxFilePath = "Assets/shader/DrawShadowMap.fx";
		if (g_renderingEngine->IsSoftShadow()) {
			modelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32_FLOAT;
		}
		else {
			modelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32_FLOAT;
		}

		if (m_isEnableInstancingDraw) {
			// インスタンシング描画を行う場合は、拡張SRVにインスタンシング描画用のデータを設定する。
			modelInitData.m_expandShaderResoruceView[0] = &m_worldMatrixArraySB;
		}

		for (int ligNo = 0; ligNo < MAX_DIRECTIONAL_LIGHT; ligNo++)
		{
			ConstantBuffer* cameraParamCBArray = m_drawShadowMapCameraParamCB[ligNo];
			Model* shadowModelArray = m_shadowModels[ligNo];
			for (int shadowMapNo = 0; shadowMapNo < NUM_SHADOW_MAP; shadowMapNo++) {
				cameraParamCBArray[shadowMapNo].Init(sizeof(Vector4), nullptr);
				modelInitData.m_expandConstantBuffer = &cameraParamCBArray[shadowMapNo];
				modelInitData.m_expandConstantBufferSize = sizeof(Vector4);
				shadowModelArray[shadowMapNo].Init(modelInitData);
			}
		}
	}

	void ModelRender::InitModelOnZprepass(
		RenderingEngine& renderingEngine,
		const char* tkmFilePath,
		EnModelUpAxis modelUpAxis
	)
	{
		ModelInitData modelInitData;
		modelInitData.m_tkmFilePath = tkmFilePath;
		modelInitData.m_fxFilePath = "Assets/shader/ZPrepass.fx";
		modelInitData.m_modelUpAxis = modelUpAxis;

		// 頂点シェーダーのエントリーポイントをセットアップ。
		SetupVertexShaderEntryPointFunc(modelInitData);

		if (m_animationClips != nullptr) {
			//スケルトンを指定する。
			modelInitData.m_skeleton = &m_skeleton;
		}

		modelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		if (m_isEnableInstancingDraw) {
			// インスタンシング描画を行う場合は、拡張SRVにインスタンシング描画用のデータを設定する。
			modelInitData.m_expandShaderResoruceView[0] = &m_worldMatrixArraySB;
		}

		m_zprepassModel.Init(modelInitData);
	}
	void ModelRender::UpdateInstancingData(const Vector3& pos, const Quaternion& rot, const Vector3& scale)
	{
		K2_ASSERT(m_numInstance < m_maxInstance, "インスタンスの数が多すぎです。");
		if (!m_isEnableInstancingDraw) {
			return;
		}
		auto wlorldMatrix = m_zprepassModel.CalcWorldMatrix(pos, rot, scale);

		// インスタンシング描画を行う。
		m_worldMatrixArray[m_numInstance] = wlorldMatrix;
		if (m_numInstance == 0) {
			//インスタンス数が0の場合のみアニメーション関係の更新を行う。
			// スケルトンを更新。
			// 各インスタンスのワールド空間への変換は、
			// インスタンスごとに行う必要があるので、頂点シェーダーで行う。
			// なので、単位行列を渡して、モデル空間でボーン行列を構築する。
			m_skeleton.Update(g_matIdentity);
			//アニメーションを進める。
			m_animation.Progress(g_gameTime->GetFrameDeltaTime() * m_animationSpeed);
		}
		m_numInstance++;
	}

	void ModelRender::Update()
	{
		if (m_isEnableInstancingDraw) {
			return;
		}


		m_zprepassModel.UpdateWorldMatrix(m_position, m_rotation, m_scale);
		if (m_renderToGBufferModel.IsInited()) {
			m_renderToGBufferModel.UpdateWorldMatrix(m_position, m_rotation, m_scale);
		}
		if (m_forwardRenderModel.IsInited()) {
			m_forwardRenderModel.UpdateWorldMatrix(m_position, m_rotation, m_scale);
		}
		if (m_translucentModel.IsInited()) {
			m_translucentModel.UpdateWorldMatrix(m_position, m_rotation, m_scale);
		}
		for (auto& models : m_shadowModels) {
			for (auto& model : models) {
				model.UpdateWorldMatrix(m_position, m_rotation, m_scale);
			}
		}

		if (m_skeleton.IsInited()) {
			m_skeleton.Update(m_zprepassModel.GetWorldMatrix());
		}

		//アニメーションを進める。
		m_animation.Progress(g_gameTime->GetFrameDeltaTime() * m_animationSpeed);

	}
	void ModelRender::Draw(RenderContext& rc)
	{
		if (m_isEnableInstancingDraw) {
			// インスタンシング描画
			m_fixNumInstanceOnFrame = 0;
			// ビューフラスタムに含まれているインスタンスのみ描画する。
			for (int instanceId = 0; instanceId < m_numInstance; instanceId++) {
				if (m_geometryDatas[instanceId].IsInViewFrustum()) {
					// ビューフラスタムに含まれている。
					m_worldMatrixArray[m_fixNumInstanceOnFrame] = m_worldMatrixArray[instanceId];
					m_fixNumInstanceOnFrame++;
				}
			}
			if (m_fixNumInstanceOnFrame != 0) {
				g_renderingEngine->AddRenderObject(this);
				m_worldMatrixArraySB.Update(m_worldMatrixArray.get());
			}
			m_numInstance = 0;
		}
		else {
			// 通常描画
			if (m_geometryDatas[0].IsInViewFrustum()) {
				// ビューフラスタムに含まれている。
				g_renderingEngine->AddRenderObject(this);
				m_fixNumInstanceOnFrame = 1;
			}
		}
	}
	void ModelRender::OnRenderShadowMap(
		RenderContext& rc,
		int ligNo,
		int shadowMapNo,
		const Matrix& lvpMatrix)
	{

		if (m_isShadowCaster) {
			Vector4 cameraParam;
			cameraParam.x = g_camera3D->GetNear();
			cameraParam.y = g_camera3D->GetFar();
			m_drawShadowMapCameraParamCB[ligNo][shadowMapNo].CopyToVRAM(cameraParam);
			m_shadowModels[ligNo][shadowMapNo].Draw(
				rc,
				g_matIdentity,
				lvpMatrix,
				m_fixNumInstanceOnFrame
			);
		}
	}

	void ModelRender::OnZPrepass(RenderContext& rc)
	{
		m_zprepassModel.Draw(rc, m_fixNumInstanceOnFrame);
	}
	void ModelRender::OnRenderToGBuffer(RenderContext& rc)
	{
		if (m_renderToGBufferModel.IsInited()) {
			m_renderToGBufferModel.Draw(rc, m_fixNumInstanceOnFrame);
		}
	}
	void ModelRender::OnForwardRender(RenderContext& rc)
	{
		if (m_forwardRenderModel.IsInited()) {
			m_forwardRenderModel.Draw(rc, m_fixNumInstanceOnFrame);
		}
	}
	void ModelRender::OnTlanslucentRender(RenderContext& rc)
	{
		if (m_translucentModel.IsInited()) {
			m_translucentModel.Draw(rc, m_fixNumInstanceOnFrame);
		}
	}
}