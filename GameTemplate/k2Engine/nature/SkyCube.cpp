#include "k2EnginePreCompile.h"
#include "SkyCube.h"

namespace nsK2Engine {

	SkyCube::SkyCube()
	{

	}

	SkyCube::~SkyCube()
	{

	}

	bool SkyCube::Start()
	{
		ModelInitData initData;
		//tkm�t�@�C���̃t�@�C���p�X���w�肷��B
		initData.m_tkmFilePath = "Assets/modelData/preset/sky.tkm";
		//�V�F�[�_�[�t�@�C���̃t�@�C���p�X���w�肷��B
		initData.m_fxFilePath = "Assets/shader/SkyCubeMap.fx";
		initData.m_vsEntryPointFunc = "VSMain";
		initData.m_psEntryPointFunc = "PSMain";
		m_texture.InitFromDDSFile(L"Assets/modelData/preset/skyCubeMap.dds");


		initData.m_expandShaderResoruceView[0] = &m_texture;
		m_modelRender.InitForwardRendering(initData);
		m_modelRender.SetShadowCasterFlag(false);
		m_modelRender.SetTRS(m_position, g_quatIdentity, m_scale);
		m_modelRender.Update();
		return true;
	}

	void SkyCube::Update()
	{
		if (m_isDirty)
		{
			m_modelRender.SetTRS(m_position, g_quatIdentity, m_scale);
			m_modelRender.Update();
			m_isDirty = false;
		}
	}

	void SkyCube::Render(RenderContext& rc)
	{
		m_modelRender.Draw(rc);
	}
}