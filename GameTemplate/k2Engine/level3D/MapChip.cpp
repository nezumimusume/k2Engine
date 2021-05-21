/*!
 *@brief	マップチップ
 */
#include "k2EnginePreCompile.h"
#include "MapChip.h"
#include "Level.h"

namespace tkEngine{
	CMapChip::CMapChip(const LevelObjectData& objData, CMapChipRender* mapChipRender)
	{
		char objName[256];
		wcstombs(objName, objData.name, 256);
		
		//ファイルパスを作成。
		wchar_t filePath[256];
		swprintf_s(filePath, L"modelData/%s.cmo", objData.name);
		m_mapChipRender = mapChipRender;
		m_physicsStaticObject.CreateMesh( 
			objData.position, 
			objData.rotation,
			objData.scale,
			mapChipRender->GetSkinModelRender()
		);
	}
}
