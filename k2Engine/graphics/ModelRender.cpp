#include "k2EnginePreCompile.h"
#include "ModelRender.h"

void ModelRender::Init(const char* filePath,
	AnimationClip* animationClips,
	int numAnimationClips,
	EnModelUpAxis enModelUpAxis)
{
	InitSkeleton(filePath);
	InitAnimation(animationClips, numAnimationClips, enModelUpAxis);
	InitModel(filePath,enModelUpAxis);
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

void ModelRender::InitModel(const char* filePath, EnModelUpAxis enModelUpAxis)
{
	ModelInitData initData;
	//tkmファイルのファイルパスを指定する。
	initData.m_tkmFilePath = filePath;
	//シェーダーファイルのファイルパスを指定する。
	initData.m_fxFilePath = "Assets/shader/model.fx";
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

	//作成した初期化データをもとにモデルを初期化する、
	m_model.Init(initData);
}

void ModelRender::Update()
{
	//スケルトンを更新。
	m_skeleton.Update(m_model.GetWorldMatrix());

	//モデルの更新。
	m_model.UpdateWorldMatrix(m_position, m_rotation, m_scale);

	//アニメーションを進める。
	m_animation.Progress(g_gameTime->GetFrameDeltaTime());
}

void ModelRender::Draw(RenderContext& rc)
{
	m_model.Draw(rc);
}
