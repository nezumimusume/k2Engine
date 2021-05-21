/*!
 *@brief	レベル。
 */

#include "k2EnginePreCompile.h"
#include "Level.h"
#include "MapChip.h"
#include "MapChipRender.h"

namespace tkEngine{
	CLevel::~CLevel()
	{
		for (auto mapChipRender : m_mapChipRenderPtrs) {
			DeleteGO(mapChipRender.second);
		}
	}
	CMapChipRender* CLevel::CreateMapChipRenderOrAddRenderObject(const LevelObjectData& objData)
	{
		WNameKey nameKey(objData.name);

		auto itFind = m_mapChipRenderPtrs.find(nameKey.GetHashCode());
		CMapChipRender* pMapChipRender = nullptr;
		if (itFind == m_mapChipRenderPtrs.end()) {
			//登録されていないオブジェクト。
			auto mapChipRender = NewGO<CMapChipRender>(0);
			pMapChipRender = mapChipRender;
			m_mapChipRenderPtrs.insert({ nameKey.GetHashCode(),mapChipRender });
		}
		else {
			//描画すべきオブジェクトのインクリメント。
			pMapChipRender = itFind->second;
		}
		pMapChipRender->AddRenderObject(objData);
		return pMapChipRender;
	}
	void CLevel::Init( 
		const wchar_t* filePath, 
		std::function<bool(LevelObjectData& objData)> hookFunc
	)
	{
		//スケルトンをロードする。
		CSkeleton skeleton;
		skeleton.Load(filePath);

		//構築構築。
		//0番目はルートオブジェクトなので飛ばす。
		for (auto i = 1; i < skeleton.GetNumBones(); i++) {
			//骨を取得。
			auto bone = skeleton.GetBone(i);
			if (bone->GetParentId() == 0) {	//親がルートの場合だけマップチップを生成する。
				LevelObjectData objData;
				CVector3 scale;
				bone->CalcWorldTRS(objData.position, objData.rotation, objData.scale);
				//3dsMaxとは軸が違うので、補正を入れる。
				auto t = objData.position.y;
				objData.position.y = objData.position.z;
				objData.position.z = -t;

				t = objData.rotation.y;
				objData.rotation.y = objData.rotation.z;
				objData.rotation.z = -t;
				objData.name = bone->GetName();
				
				std::swap(objData.scale.y, objData.scale.z);

				auto isHook = false;
				if (hookFunc != nullptr) {
					//hook関数が指定されているのでhook関数を呼び出す。
					isHook = hookFunc(objData);
				}
				if (isHook == false) {
					//マップチップレンダラーを作成する。
					CreateMapChipRenderOrAddRenderObject(objData);
				}
			}
		}
		//マップチップレンダラーを初期化する。
		for (auto& mapChipRender : m_mapChipRenderPtrs) {
			mapChipRender.second->InitAfterAddAllRenderObjects();
			mapChipRender.second->QueryRenderObjDatas([&](const LevelObjectData& objData) {
				//フックされなかったので、マップチップを作成する。
				auto mapChip = std::make_unique<CMapChip>(objData, mapChipRender.second);
				m_mapChipPtrs.push_back(std::move(mapChip));
			});
		}
	}	
}