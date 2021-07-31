#include "k2EnginePreCompile.h"
#include "ModelRender.h"
#include "RenderingEngine.h"

void ModelRender::SetupVertexShaderEntryPointFunc(ModelInitData& modelInitData)
{
	if (m_isEnableInstancingDraw) {
		//�C���X�^���V���O�`��B
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
	//�C���X�^���V���O�`��p�̃f�[�^���������B
	InitInstancingDraw(1);
	InitSkeleton(filePath);
	InitAnimation(animationClips, numAnimationClips, enModelUpAxis);
	ModelInitData initData;
	//tkm�t�@�C���̃t�@�C���p�X���w�肷��B
	initData.m_tkmFilePath = filePath;
	//�V�F�[�_�[�t�@�C���̃t�@�C���p�X���w�肷��B
	initData.m_fxFilePath = "Assets/shader/Model.fx";
	//�m���X�L�����b�V���p�̒��_�V�F�[�_�[�̃G���g���[�|�C���g���w�肷��B
	initData.m_vsEntryPointFunc = "VSMain";

	if (m_animationClips != nullptr) {
		//�X�L�����b�V���p�̒��_�V�F�[�_�[�̃G���g���[�|�C���g���w��B
		initData.m_vsSkinEntryPointFunc = "VSSkinMain";
		//�X�P���g�����w�肷��B
		initData.m_skeleton = &m_skeleton;
	}

	//���f���̏�������w�肷��B
	initData.m_modelUpAxis = enModelUpAxis;
	
	initData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	if (m_isEnableInstancingDraw) {
		// �C���X�^���V���O�`����s���ꍇ�́A�g��SRV�ɃC���X�^���V���O�`��p�̃f�[�^��ݒ肷��B
		initData.m_expandShaderResoruceView[0] = &m_worldMatrixArraySB;
	}
	//�쐬�����������f�[�^�����ƂɃ��f��������������A
	m_forwardRenderModel.Init(initData);
	//�C���X�^���V���O�`��p�̃f�[�^���������B
	InitInstancingDraw(maxInstance);
	//ZPrepass�`��p�̃��f�����������B
	InitModelOnZprepass(*g_renderingEngine, filePath, enModelUpAxis);
	//�V���h�E�}�b�v�`��p�̃��f�����������B
	InitModelOnShadowMap(*g_renderingEngine, filePath, enModelUpAxis);
	//�o�E���f�B���O�{�����[�����������B
	InitBoundingVolume();
}

void ModelRender::InitForwardRendering(ModelInitData& initData)
{
	//�C���X�^���V���O�`��p�̃f�[�^���������B
	InitInstancingDraw(1);
	InitSkeleton(initData.m_tkmFilePath);

	initData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//�쐬�����������f�[�^�����ƂɃ��f��������������B
	m_forwardRenderModel.Init(initData);

	//ZPrepass�`��p�̃��f�����������B
	InitModelOnZprepass(*g_renderingEngine, initData.m_tkmFilePath, initData.m_modelUpAxis);
	//�V���h�E�}�b�v�`��p�̃��f�����������B
	InitModelOnShadowMap(*g_renderingEngine, initData.m_tkmFilePath, initData.m_modelUpAxis);

	//�o�E���f�B���O�{�����[�����������B
	InitBoundingVolume();
}

void ModelRender::Init(const char* filePath,
	AnimationClip* animationClips,
	int numAnimationClips,
	EnModelUpAxis enModelUpAxis,
	bool isShadowReciever,
	int maxInstance)
{
	// �C���X�^���V���O�`��p�̃f�[�^���������B
	InitInstancingDraw(maxInstance);
	// �X�P���g�����������B
	InitSkeleton(filePath);
	// �A�j���[�V�������������B
	InitAnimation(animationClips, numAnimationClips, enModelUpAxis);
	// GBuffer�`��p�̃��f�����������B
	InitModelOnRenderGBuffer(*g_renderingEngine, filePath, enModelUpAxis, isShadowReciever);
	// ZPrepass�`��p�̃��f�����������B
	InitModelOnZprepass(*g_renderingEngine, filePath, enModelUpAxis);
	// �V���h�E�}�b�v�`��p�̃��f�����������B
	InitModelOnShadowMap(*g_renderingEngine, filePath, enModelUpAxis);
	// �o�E���f�B���O�{�����[�����������B
	InitBoundingVolume();
}

void ModelRender::InitSkeleton(const char* filePath)
{
	//�X�P���g���̃f�[�^��ǂݍ��݁B
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
		//�C���X�^���V���O�`����s���̂ŁA
		//����p�̃f�[�^���\�z����B
		//���[���h�s��̔z��̃��������m�ۂ���B
		m_worldMatrixArray = std::make_unique<Matrix[]>(m_maxInstance);
		//���[���h�s���GPU�ɓ]�����邽�߂̃X�g���N�`���[�h�o�b�t�@���m�ہB
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

	// ���_�V�F�[�_�[�̃G���g���[�|�C���g���Z�b�g�A�b�v�B
	SetupVertexShaderEntryPointFunc(modelInitData);

	if (m_animationClips != nullptr) {
		//�X�P���g�����w�肷��B
		modelInitData.m_skeleton = &m_skeleton;
	}

	if (isShadowReciever) {
		modelInitData.m_psEntryPointFunc = "PSMainShadowReciever";
	}
	//���f���̏�������w�肷��B
	modelInitData.m_modelUpAxis = enModelUpAxis;

	modelInitData.m_tkmFilePath = tkmFilePath;
	modelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	modelInitData.m_colorBufferFormat[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	modelInitData.m_colorBufferFormat[2] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	modelInitData.m_colorBufferFormat[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	modelInitData.m_colorBufferFormat[4] = DXGI_FORMAT_R8G8B8A8_UNORM;
	if (m_isEnableInstancingDraw) {
		// �C���X�^���V���O�`����s���ꍇ�́A�g��SRV�ɃC���X�^���V���O�`��p�̃f�[�^��ݒ肷��B
		modelInitData.m_expandShaderResoruceView[0] = &m_worldMatrixArraySB;
	}
	m_renderToGBufferModel.Init(modelInitData);

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
	// ���_�V�F�[�_�[�̃G���g���[�|�C���g���Z�b�g�A�b�v�B
	SetupVertexShaderEntryPointFunc(modelInitData);

	if (m_animationClips != nullptr) {
		//�X�P���g�����w�肷��B
		modelInitData.m_skeleton = &m_skeleton;
	}

	modelInitData.m_fxFilePath = "Assets/shader/DrawShadowMap.fx";
	modelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32_FLOAT;
	
	if (m_isEnableInstancingDraw) {
		// �C���X�^���V���O�`����s���ꍇ�́A�g��SRV�ɃC���X�^���V���O�`��p�̃f�[�^��ݒ肷��B
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

	// ���_�V�F�[�_�[�̃G���g���[�|�C���g���Z�b�g�A�b�v�B
	SetupVertexShaderEntryPointFunc(modelInitData);

	if (m_animationClips != nullptr) {
		//�X�P���g�����w�肷��B
		modelInitData.m_skeleton = &m_skeleton;
	}

	modelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	if (m_isEnableInstancingDraw) {
		// �C���X�^���V���O�`����s���ꍇ�́A�g��SRV�ɃC���X�^���V���O�`��p�̃f�[�^��ݒ肷��B
		modelInitData.m_expandShaderResoruceView[0] = &m_worldMatrixArraySB;
	}

	m_zprepassModel.Init(modelInitData);
}
void ModelRender::UpdateInstancingData(const Vector3& pos, const Quaternion& rot, const Vector3& scale)
{
	if (!m_isEnableInstancingDraw) {
		return;
	}
	auto wlorldMatrix = m_zprepassModel.CalcWorldMatrix(pos, rot, scale);
	// �C���X�^���V���O�`��͂����Ńr���[�J�����O���s���B
	if ( IsViewCulling(wlorldMatrix)) {
		return;
	}
	// �C���X�^���V���O�`����s���B
	m_worldMatrixArray[m_numInstance] = wlorldMatrix;
	if (m_numInstance == 0) {
		//�C���X�^���X����0�̏ꍇ�̂݃A�j���[�V�����֌W�̍X�V���s���B
		// �X�P���g�����X�V�B
		// �e�C���X�^���X�̃��[���h��Ԃւ̕ϊ��́A
		// �C���X�^���X���Ƃɍs���K�v������̂ŁA���_�V�F�[�_�[�ōs���B
		// �Ȃ̂ŁA�P�ʍs���n���āA���f����ԂŃ{�[���s����\�z����B
		m_skeleton.Update(g_matIdentity);
		//�A�j���[�V������i�߂�B
		m_animation.Progress(g_gameTime->GetFrameDeltaTime() * m_animationSpeed);
	}
	m_numInstance++;
	
}
void ModelRender::InitBoundingVolume()
{
	// tkm�t�@�C�����烂�f���̍ŏ����W�ƍő���W�𒲂ׂ�B
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
	
	//�A�j���[�V������i�߂�B
	m_animation.Progress(g_gameTime->GetFrameDeltaTime() * m_animationSpeed);

	
}
bool ModelRender::IsViewCulling(const Matrix& mWorld)
{
	// AABB���\������8���_�̃��[���h���W���v�Z����B
	Vector3 worldPos[8];
	m_aabb.CalcVertexPositions(
		worldPos,
		mWorld
	);

	Vector4 vMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
	Vector4 vMin = {  FLT_MAX,  FLT_MAX,  FLT_MAX, FLT_MAX };
	const auto& viewProjMatrix = g_camera3D->GetViewProjectionMatrix();

	for (Vector4 v : worldPos) {
		
		viewProjMatrix.Apply(v);
		v.x /= v.w;
		v.y /= v.w;
		v.z /= v.w;

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
		//��ʂɉf��B
		return false;
	}
	// �J�����O����Ă���̂ŁA��ʂɉf��Ȃ��B
	return true;

}
void ModelRender::Draw(RenderContext& rc)
{
	// �r���[�J�����O���s���B
	if ( m_isEnableInstancingDraw == false
		&& IsViewCulling(m_zprepassModel.GetWorldMatrix())
	) {
		return;
	}
	
	g_renderingEngine->AddRenderObject(this);
	if (m_isEnableInstancingDraw) {
		// ���̃t���[���ŕ`�悷��C���X�^���X�̐����m�肷��B
		m_fixNumInstanceOnFrame = m_numInstance;
		// �C���X�^���X�̐��v���p�̕ϐ����N���A�B
		m_numInstance = 0;
		// ���[���h�s��̔z����X�g���N�`���[�h�o�b�t�@�ɃR�s�[�B
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