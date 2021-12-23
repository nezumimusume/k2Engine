#include "k2EngineLowPreCompile.h"
#include "Model.h"
#include "Material.h"

namespace nsK2EngineLow {
	void Model::Init(const ModelInitData& initData)
	{
		/* ���C�g�������̏������̎��ɂ�m_fxFilePath�͎w�肳��Ă��Ȃ��ꍇ������̂ŃX���[����B
		MY_ASSERT(
			initData.m_fxFilePath,
			"error : initData.m_fxFilePath���w�肳��Ă��܂���B"
		);
		*/
		K2_ASSERT(
			initData.m_tkmFilePath,
			"error : initData.m_tkmFilePath���w�肳��Ă��܂���B"
		);


		if (initData.m_skeleton != nullptr) {
			//�X�P���g�����w�肳��Ă���B
			m_meshParts.BindSkeleton(*initData.m_skeleton);
		}

		m_modelUpAxis = initData.m_modelUpAxis;

		auto tkmFile = g_engine->GetTkmFileFromBank(initData.m_tkmFilePath);
		if (tkmFile == nullptr) {
			//���o�^
			tkmFile = new TkmFile;
			tkmFile->Load(initData.m_tkmFilePath, false);
			g_engine->RegistTkmFileToBank(initData.m_tkmFilePath, tkmFile);
		}
		m_tkmFile = tkmFile;
		m_meshParts.InitFromTkmFile(
			*m_tkmFile,
			initData.m_fxFilePath,
			initData.m_vsEntryPointFunc,
			initData.m_vsSkinEntryPointFunc,
			initData.m_psEntryPointFunc,
			initData.m_expandConstantBuffer,
			initData.m_expandConstantBufferSize,
			initData.m_expandShaderResoruceView,
			initData.m_colorBufferFormat,
			initData.m_alphaBlendMode,
			initData.m_isDepthWrite,
			initData.m_isDepthTest,
			initData.m_cullMode
		);

		UpdateWorldMatrix(g_vec3Zero, g_quatIdentity, g_vec3One);
		m_isInited = true;

	}
	void Model::ReInitMaterials(MaterialReInitData& reInitData)
	{
		m_meshParts.ReInitMaterials(reInitData);
	}

	Matrix Model::CalcWorldMatrix(Vector3 pos, Quaternion rot, Vector3 scale)
	{
		Matrix mBias, mWorld;
		if (m_modelUpAxis == enModelUpAxisZ) {
			//Z-up
			mBias.MakeRotationX(Math::PI * -0.5f);
		}
		Matrix mTrans, mRot, mScale;
		mTrans.MakeTranslation(pos);
		mRot.MakeRotationFromQuaternion(rot);
		mScale.MakeScaling(scale);
		mWorld = mBias * mScale * mRot * mTrans;
		return mWorld;
	}
	//todo �������ύX����K�v����H�H
	void Model::ChangeAlbedoMap(const char* materialName, Texture& albedoMap)
	{
		m_meshParts.QueryMeshs([&](const SMesh& mesh) {
			//todo �}�e���A������tkm�t�@�C���ɏo�͂��ĂȂ������B
			//todo ���͑S�}�e���A�������ւ��܂�
			for (Material* material : mesh.m_materials) {
				material->GetAlbedoMap().InitFromD3DResource(albedoMap.Get());
			}
		});
		//�f�B�X�N���v�^�q�[�v�̍č쐬�B
		m_meshParts.CreateDescriptorHeaps();

	}
	void Model::Draw(
		RenderContext& rc,
		int numInstance
	)
	{
		m_meshParts.Draw(
			rc,
			m_worldMatrix,
			g_camera3D->GetViewMatrix(),
			g_camera3D->GetProjectionMatrix(),
			numInstance
		);
	}
	void Model::Draw(
		RenderContext& rc,
		Camera& camera,
		int numInstance
	)
	{
		Draw(
			rc,
			camera.GetViewMatrix(),
			camera.GetProjectionMatrix(),
			numInstance
		);
	}

	void Model::Draw(
		RenderContext& rc,
		const Matrix& viewMatrix,
		const Matrix& projMatrix,
		int numInstance
	)
	{
		if (numInstance == 0) {
			// �C���X�^���X�̐���0�Ȃ̂Ń��^�[���B
			return;
		}
		m_meshParts.Draw(
			rc,
			m_worldMatrix,
			viewMatrix,
			projMatrix,
			numInstance
		);
	}
}