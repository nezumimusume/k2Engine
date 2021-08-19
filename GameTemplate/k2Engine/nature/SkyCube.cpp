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
		//tkmファイルのファイルパスを指定する。
		initData.m_tkmFilePath = "Assets/modelData/preset/sky.tkm";
		//シェーダーファイルのファイルパスを指定する。
		initData.m_fxFilePath = "Assets/shader/SkyCubeMap.fx";
		initData.m_vsEntryPointFunc = "VSMain";
		initData.m_psEntryPointFunc = "PSMain";
		m_texture[enSkyCubeType_Day].InitFromDDSFile(L"Assets/modelData/preset/skyCubeMap.dds");
		m_texture[enSkyCubeType_Night].InitFromDDSFile(L"Assets/modelData/preset/skyCubeMapNight.dds");
		m_texture[enSkyCubeType_Snow].InitFromDDSFile(L"Assets/modelData/preset/skyCubeMapSnow.dds");
		m_texture[enSkyCubeType_Snow_2].InitFromDDSFile(L"Assets/modelData/preset/skyCubeMapSnow_2.dds");
		m_texture[enSkyCubeType_Wild].InitFromDDSFile(L"Assets/modelData/preset/skyCubeMapWild.dds");
		m_texture[enSkyCubeType_Wild_Night].InitFromDDSFile(L"Assets/modelData/preset/skyCubeMapWild_Night.dds");
		m_texture[enSkyCubeType_Wild_2].InitFromDDSFile(L"Assets/modelData/preset/skyCubeMapWild_2.dds");
		m_texture[enSkyCubeType_Grass].InitFromDDSFile(L"Assets/modelData/preset/skyCubeMapGrass.dds");
		m_texture[enSkyCubeType_Euro].InitFromDDSFile(L"Assets/modelData/preset/skyCubeMapEuro.dds");
		m_texture[enSkyCubeType_DayToon].InitFromDDSFile(L"Assets/modelData/preset/skyCubeMapDay_Toon.dds");
		m_texture[enSkyCubeType_DayToon_2].InitFromDDSFile(L"Assets/modelData/preset/skyCubeMapDay_Toon_02.dds");
		m_texture[enSkyCubeType_DayToon_3].InitFromDDSFile(L"Assets/modelData/preset/skyCubeMapDay_Toon_03.dds");
		m_texture[enSkyCubeType_DayToon_4].InitFromDDSFile(L"Assets/modelData/preset/skyCubeMapDay_Toon_04.dds");
		m_texture[enSkyCubeType_NightToon].InitFromDDSFile(L"Assets/modelData/preset/skyCubeMapNight_Toon.dds");
		m_texture[enSkyCubeType_NightToon_2].InitFromDDSFile(L"Assets/modelData/preset/skyCubeMapNight_Toon_02.dds");
		m_texture[enSkyCubeType_SunriseToon].InitFromDDSFile(L"Assets/modelData/preset/skyCubeMapSunrise_Toon.dds");
		m_texture[enSkyCubeType_SpaceToon].InitFromDDSFile(L"Assets/modelData/preset/skyCubeMapSpace_Toon.dds");
		m_texture[enSkyCubeType_SpaceToon_2].InitFromDDSFile(L"Assets/modelData/preset/skyCubeMapSpace_Toon_2.dds");

		initData.m_expandShaderResoruceView[0] = &m_texture[m_type];
		initData.m_expandConstantBuffer = &m_luminance;
		initData.m_expandConstantBufferSize = sizeof(m_luminance);
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