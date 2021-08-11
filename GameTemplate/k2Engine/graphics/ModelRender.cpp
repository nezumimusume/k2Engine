#include "k2EnginePreCompile.h"
#include "ModelRender.h"
#include "RenderingEngine.h"

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
void ModelRender::InitForwardRendering(const char* filePath,
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
	//フォワードレンダリング用のモデルを初期化。
	InitModelOnForward(*g_renderingEngine, filePath, enModelUpAxis, isShadowReciever);
	//ZPrepass描画用のモデルを初期化。
	InitModelOnZprepass(*g_renderingEngine, filePath, enModelUpAxis);
	//シャドウマップ描画用のモデルを初期化。
	InitModelOnShadowMap(*g_renderingEngine, filePath, enModelUpAxis);
	//バウンディングボリュームを初期化。
	InitBoundingVolume();
}

void ModelRender::InitForwardRendering(ModelInitData& initData)
{
	//インスタンシング描画用のデータを初期化。
	InitInstancingDraw(1);
	InitSkeleton(initData.m_tkmFilePath);

	initData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//作成した初期化データをもとにモデルを初期化する。
	m_forwardRenderModel.Init(initData);

	//ZPrepass描画用のモデルを初期化。
	InitModelOnZprepass(*g_renderingEngine, initData.m_tkmFilePath, initData.m_modelUpAxis);
	//シャドウマップ描画用のモデルを初期化。
	InitModelOnShadowMap(*g_renderingEngine, initData.m_tkmFilePath, initData.m_modelUpAxis);

	//バウンディングボリュームを初期化。
	InitBoundingVolume();
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
	// バウンディングボリュームを初期化。
	InitBoundingVolume();
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
	modelInitData.m_colorBufferFormat[2] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	modelInitData.m_colorBufferFormat[3] = DXGI_FORMAT_R8G8B8A8_UNORM;
	modelInitData.m_colorBufferFormat[4] = DXGI_FORMAT_R8G8B8A8_UNORM;
	modelInitData.m_alphaBlendMode = AlphaBlendMode_None;
	if (m_isEnableInstancingDraw) {
		// インスタンシング描画を行う場合は、拡張SRVにインスタンシング描画用のデータを設定する。
		modelInitData.m_expandShaderResoruceView[0] = &m_worldMatrixArraySB;
	}
	m_renderToGBufferModel.Init(modelInitData);

}

void ModelRender::InitModelOnForward(
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
	//モデルの上方向を指定する。
	modelInitData.m_modelUpAxis = enModelUpAxis;

	modelInitData.m_tkmFilePath = tkmFilePath;
	modelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	if (m_isEnableInstancingDraw) {
		// インスタンシング描画を行う場合は、拡張SRVにインスタンシング描画用のデータを設定する。
		modelInitData.m_expandShaderResoruceView[0] = &m_worldMatrixArraySB;
	}
	m_forwardRenderModel.Init(modelInitData);
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
	};

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
	MY_ASSERT(m_numInstance < m_maxInstance, "インスタンスの数が多すぎです。");
	if (!m_isEnableInstancingDraw) {
		return;
	}
	auto wlorldMatrix = m_zprepassModel.CalcWorldMatrix(pos, rot, scale);
	if (m_numInstance == 0) {
		// AABBの情報クリア。
		m_aabbMax = { -FLT_MIN, -FLT_MIN, -FLT_MIN };
		m_aabbMin = { FLT_MAX, FLT_MAX, FLT_MAX };
	}
	// インスタンシング描画はここでビューカリングを行う。
	if ( IsViewCulling(wlorldMatrix)) {
		return;
	}
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
void ModelRender::InitBoundingVolume()
{
	// tkmファイルからモデルの最小座標と最大座標を調べる。
	Vector3 vMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	Vector3 vMin = {  FLT_MAX,  FLT_MAX,  FLT_MAX };

	const auto& tkmFile = m_zprepassModel.GetTkmFile();
	tkmFile.QueryMeshParts([&](const TkmFile::SMesh& mesh) {
		for (const auto& vertex : mesh.vertexBuffer) {
			vMax.Max( vertex.pos );
			vMin.Min( vertex.pos );
		}
	});
	m_aabb.Init(vMax, vMin);
}
void ModelRender::Update()
{
	if (m_isEnableInstancingDraw) {
		return;
	}
	// 更新関数が呼ばれたAABBの情報をクリア。
	m_aabbMax = { -FLT_MIN, -FLT_MIN, -FLT_MIN };
	m_aabbMin = {  FLT_MAX,  FLT_MAX,  FLT_MAX };

	m_zprepassModel.UpdateWorldMatrix(m_position, m_rotation, m_scale);
	if (m_renderToGBufferModel.IsInited()) {
		m_renderToGBufferModel.UpdateWorldMatrix(m_position, m_rotation, m_scale);
	}
	if (m_forwardRenderModel.IsInited()) {
		m_forwardRenderModel.UpdateWorldMatrix(m_position, m_rotation, m_scale);
	}
	for (auto& models : m_shadowModels) {
		for (auto& model : models) {
			model.UpdateWorldMatrix(m_position, m_rotation, m_scale);
		}
	}
	
	if (m_skeleton.IsInited()) {
		if (m_renderToGBufferModel.IsInited())
		{
			m_skeleton.Update(m_renderToGBufferModel.GetWorldMatrix());
		}
		if (m_forwardRenderModel.IsInited())
		{
			m_skeleton.Update(m_forwardRenderModel.GetWorldMatrix());
		}
	}
	
	//アニメーションを進める。
	m_animation.Progress(g_gameTime->GetFrameDeltaTime() * m_animationSpeed);

}
bool ModelRender::IsViewCulling(const Matrix& mWorld)
{
	// AABBを構成する8頂点のワールド座標を計算する。
	Vector3 worldPos[8];
	m_aabb.CalcVertexPositions(
		worldPos,
		mWorld
	);

	Vector4 vMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
	Vector4 vMin = {  FLT_MAX,  FLT_MAX,  FLT_MAX, FLT_MAX };
	
	const auto& viewProjMatrix = g_renderingEngine->GetViewProjectionMatrixForViewCulling();
	
	for (Vector4 v : worldPos) {
		// シーンのジオメトリ情報を追加。
		m_aabbMax.Max({ v.x, v.y, v.z });
		m_aabbMin.Min({ v.x, v.y, v.z });

		viewProjMatrix.Apply(v);
		v.x /= fabsf(v.w);
		v.y /= fabsf(v.w);
		v.z /= fabsf(v.w);

		vMax.Max(v);
		vMin.Min(v);
	}

	if (vMax.x > -1.0f
		&& vMax.y > -1.0f
		&& vMax.z > 0.0f
		&& vMin.x < 1.0f
		&& vMin.y < 1.0f
		&& vMin.z < 1.0f
		) {
		//画面に映る。
		return false;
	}
	// カリングされているので、画面に映らない。
	return true;

}

void ModelRender::Draw(RenderContext& rc)
{
	// ビューカリングを行う。
	if ( m_isEnableInstancingDraw == false 
		&& IsViewCulling(m_zprepassModel.GetWorldMatrix())
	) {
		return;
	}
	
	g_renderingEngine->AddRenderObject(this);
	if (m_isEnableInstancingDraw) {
		// このフレームで描画するインスタンスの数を確定する。
		m_fixNumInstanceOnFrame = m_numInstance;
		// インスタンスの数計測用の変数をクリア。
		m_numInstance = 0;
		// ワールド行列の配列をストラクチャードバッファにコピー。
		m_worldMatrixArraySB.Update(m_worldMatrixArray.get());
	}
	else {
		m_fixNumInstanceOnFrame = 1;
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