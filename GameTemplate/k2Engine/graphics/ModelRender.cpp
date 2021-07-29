#include "k2EnginePreCompile.h"
#include "ModelRender.h"
#include "RenderingEngine.h"

void ModelRender::InitForwardRendering(const char* filePath,
	AnimationClip* animationClips,
	int numAnimationClips,
	EnModelUpAxis enModelUpAxis,
	bool isShadowReciever,
	int maxInstance)
{
	//インスタンシング描画用のデータを初期化。
	InitInstancingDraw(1);
	InitSkeleton(filePath);
	InitAnimation(animationClips, numAnimationClips, enModelUpAxis);
	ModelInitData initData;
	//tkmファイルのファイルパスを指定する。
	initData.m_tkmFilePath = filePath;
	//シェーダーファイルのファイルパスを指定する。
	initData.m_fxFilePath = "Assets/shader/Model.fx";
	//ノンスキンメッシュ用の頂点シェーダーのエントリーポイントを指定する。
	initData.m_vsEntryPointFunc = "VSMain";

	if (m_animationClips != nullptr) {
		//スキンメッシュ用の頂点シェーダーのエントリーポイントを指定。
		initData.m_vsSkinEntryPointFunc = "VSSkinMain";
		//スケルトンを指定する。
		initData.m_skeleton = &m_skeleton;
	}

	//モデルの上方向を指定する。
	initData.m_modelUpAxis = enModelUpAxis;
	
	initData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	if (m_isEnableInstancingDraw) {
		// インスタンシング描画を行う場合は、拡張SRVにインスタンシング描画用のデータを設定する。
		initData.m_expandShaderResoruceView[0] = &m_worldMatrixArraySB;
	}
	//作成した初期化データをもとにモデルを初期化する、
	m_forwardRenderModel.Init(initData);
	//インスタンシング描画用のデータを初期化。
	InitInstancingDraw(maxInstance);
	//ZPrepass描画用のモデルを初期化。
	InitZprepassModel(*g_renderingEngine, filePath);
	//シャドウマップ描画用のモデルを初期化。
	InitShadowMapModel(*g_renderingEngine, filePath);

}

void ModelRender::InitForwardRendering(ModelInitData& initData)
{
	//インスタンシング描画用のデータを初期化。
	InitInstancingDraw(1);
	InitSkeleton(initData.m_tkmFilePath);

	initData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//作成した初期化データをもとにモデルを初期化する。
	m_forwardRenderModel.Init(initData);

	//todo これはあかんな。
	
	//ZPrepass描画用のモデルを初期化。
	InitZprepassModel(*g_renderingEngine, initData.m_tkmFilePath);
	//シャドウマップ描画用のモデルを初期化。
	InitShadowMapModel(*g_renderingEngine, initData.m_tkmFilePath);
}

void ModelRender::Init(const char* filePath,
	AnimationClip* animationClips,
	int numAnimationClips,
	EnModelUpAxis enModelUpAxis,
	bool isShadowReciever,
	int maxInstance)
{
	//インスタンシング描画用のデータを初期化。
	InitInstancingDraw(maxInstance);
	InitSkeleton(filePath);
	InitAnimation(animationClips, numAnimationClips, enModelUpAxis);
	ModelInitData modelInitData;
	modelInitData.m_fxFilePath = "Assets/shader/RenderToGBufferFor3DModel.fx";
	if (m_isEnableInstancingDraw) {
		//インスタンシング描画。
		modelInitData.m_vsEntryPointFunc = "VSMainInstancing";
	}
	else {
		modelInitData.m_vsEntryPointFunc = "VSMain";
	}
	if (m_animationClips != nullptr) {
		//スキンメッシュ用の頂点シェーダーのエントリーポイントを指定。
		modelInitData.m_vsSkinEntryPointFunc = "VSMainSkin";
		//スケルトンを指定する。
		modelInitData.m_skeleton = &m_skeleton;
	}
	if (isShadowReciever) {
		modelInitData.m_psEntryPointFunc = "PSMainShadowReciever";
	}
	//モデルの上方向を指定する。
	modelInitData.m_modelUpAxis = enModelUpAxis;

	modelInitData.m_tkmFilePath = filePath;
	modelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	modelInitData.m_colorBufferFormat[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	modelInitData.m_colorBufferFormat[2] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	modelInitData.m_colorBufferFormat[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	modelInitData.m_colorBufferFormat[4] = DXGI_FORMAT_R8G8B8A8_UNORM;
	if (m_isEnableInstancingDraw) {
		// インスタンシング描画を行う場合は、拡張SRVにインスタンシング描画用のデータを設定する。
		modelInitData.m_expandShaderResoruceView[0] = &m_worldMatrixArraySB;
	}
	m_renderToGBufferModel.Init(modelInitData);

	
	//ZPrepass描画用のモデルを初期化。
	InitZprepassModel(*g_renderingEngine, filePath);
	//シャドウマップ描画用のモデルを初期化。
	InitShadowMapModel(*g_renderingEngine, filePath);
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
void ModelRender::InitShadowMapModel(RenderingEngine& renderingEngine, const char* tkmFilePath)
{
	ModelInitData modelInitData;
	modelInitData.m_tkmFilePath = tkmFilePath;
	if (m_isEnableInstancingDraw) {
		modelInitData.m_vsEntryPointFunc = "VSMainInstancing";
	}

	if (m_animationClips != nullptr) {
		modelInitData.m_vsSkinEntryPointFunc = "VSSkinMain";
		//スケルトンを指定する。
		modelInitData.m_skeleton = &m_skeleton;
	}

	modelInitData.m_fxFilePath = "Assets/shader/DrawShadowMap.fx";
	modelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32_FLOAT;
	
	if (m_isEnableInstancingDraw) {
		// インスタンシング描画を行う場合は、拡張SRVにインスタンシング描画用のデータを設定する。
		modelInitData.m_expandShaderResoruceView[0] = &m_worldMatrixArraySB;
	}

	for (
		int ligNo = 0;
		ligNo < NUM_DEFERRED_LIGHTING_DIRECTIONAL_LIGHT;
		ligNo++)
	{
		m_shadowModels[ligNo][0].Init(modelInitData);
		m_shadowModels[ligNo][1].Init(modelInitData);
		m_shadowModels[ligNo][2].Init(modelInitData);
	}
}

void ModelRender::InitZprepassModel(RenderingEngine& renderingEngine, const char* tkmFilePath)
{
	ModelInitData modelInitData;
	modelInitData.m_tkmFilePath = tkmFilePath;
	modelInitData.m_fxFilePath = "Assets/shader/ZPrepass.fx";
	if (m_isEnableInstancingDraw) {
		modelInitData.m_vsEntryPointFunc = "VSMainInstancing";
	}
	if (m_animationClips != nullptr) {	
		modelInitData.m_vsSkinEntryPointFunc = "VSSkinMain";
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
	if (!m_isEnableInstancingDraw) {
		return;
	}

	// インスタンシング描画を行う。
	m_worldMatrixArray[m_numInstance] = m_zprepassModel.CalcWorldMatrix(pos, rot, scale);
	if (m_numInstance == 0) {
		//インスタンス数が0の場合のみアニメーションの更新を行う。
		//アニメーションを進める。
		m_animation.Progress(g_gameTime->GetFrameDeltaTime() * m_animationSpeed);
		//todo アニメーションの対応はひとまず後で・・・。
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

void ModelRender::Draw(RenderContext& rc)
{
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