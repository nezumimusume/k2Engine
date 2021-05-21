/*!
 *@brief	マップチップレンダラー
 */
#include "k2EnginePreCompile.h"
#include "MapChipRender.h"
#include "Level.h"

namespace tkEngine {
	CMapChipRender::CMapChipRender()
	{
	}
	CMapChipRender::~CMapChipRender()
	{
	}
	
	void CMapChipRender::Update()
	{
		if (m_renderObjDatas.size() > 1) {
			for (auto& objData : m_renderObjDatas) {
				m_modelRender.SetTRS(objData.position, objData.rotation, objData.scale);
				m_modelRender.Update();
			}
		}
	}

	void CMapChipRender::InitAfterAddAllRenderObjects()
	{
		//ファイルパスを作成。
		wchar_t filePath[256];
		swprintf_s(filePath, L"modelData/%s.cmo", m_renderObjDatas[0].name);
		
			//のっとインスタンシング
			m_modelRender->Init(filePath);
			m_modelRender->SetPosition(m_renderObjDatas[0].position);
			m_modelRender->SetRotation(m_renderObjDatas[0].rotation);
			m_modelRender->SetScale(m_renderObjDatas[0].scale);
		
		//影
		m_modelRender.SetShadowReceiverFlag(true);
		m_modelRender.SetShadowCasterFlag(true);
	}
}
