#include "k2EngineLowPreCompile.h"
#include "Model.h"
#include "Material.h"

namespace nsK2EngineLow {
	void Model::Init(const ModelInitData& initData)
	{
		/* レイトレ向けの初期化の時にはm_fxFilePathは指定されていない場合があるのでスルーする。
		MY_ASSERT(
			initData.m_fxFilePath,
			"error : initData.m_fxFilePathが指定されていません。"
		);
		*/
		K2_ASSERT(
			initData.m_tkmFilePath,
			"error : initData.m_tkmFilePathが指定されていません。"
		);


		if (initData.m_skeleton != nullptr) {
			//スケルトンが指定されている。
			m_meshParts.BindSkeleton(*initData.m_skeleton);
		}

		m_modelUpAxis = initData.m_modelUpAxis;

		auto tkmFile = g_engine->GetTkmFileFromBank(initData.m_tkmFilePath);
		if (tkmFile == nullptr) {
			//未登録
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
			initData.m_cullMode,
			initData.m_computedAnimationVertexBuffer
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
	//todo ここも変更する必要あり？？
	void Model::ChangeAlbedoMap(const char* materialName, Texture& albedoMap)
	{
		m_meshParts.QueryMeshs([&](const SMesh& mesh) {
			//todo マテリアル名をtkmファイルに出力してなかった。
			//todo 今は全マテリアル差し替えます
			for (Material* material : mesh.m_materials) {
				material->GetAlbedoMap().InitFromD3DResource(albedoMap.Get());
			}
		});
		//ディスクリプタヒープの再作成。
		m_meshParts.CreateDescriptorHeaps();
	}
	void Model::DispatchComputeAnimatedVertexBuffer(RenderContext& rc)
	{

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
			// インスタンスの数が0なのでリターン。
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