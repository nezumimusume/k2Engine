/*!
 *@brief	レベル。
 */

#include "k2EnginePreCompile.h"
#include "Level2DRender.h"
#include "Level2D/CaslFile.h"
#include "MapChip2DRender.h"

namespace nsK2Engine {
	void Level2DRender::AddMapChip2DRender(Level2DObjectData* objData)
	{
		auto mapChip2DPtr = std::make_unique<MapChip2DRender>();
		mapChip2DPtr.get()->Init(objData);
		//std::unique_ptrを移動させる時はstd::move()を使う。
		m_mapChip2DRenderPtrList.push_back(std::move(mapChip2DPtr));
	}

	void Level2DRender::Init(
		const char* filePath,
		std::function<bool(Level2DObjectData& objData)> hookFunc
	)
	{
		//Caslファイルをロードする。
		CaslFile casl;
		casl.Load(filePath);

		//構築構築。
		for (int i = 0; i < casl.GetNumCaslData(); i++)
		{
			auto caslData = casl.GetCaslData(i);
			Level2DObjectData objData;
			objData.scale.x = caslData->scale.x;
			objData.scale.y = caslData->scale.y;
			objData.position.x = caslData->position.x;
			objData.position.y = caslData->position.y;
			objData.width = caslData->width;
			objData.height = caslData->height;
			objData.name = caslData->name.get();
			objData.ddsFilePath = caslData->ddsFilePath.get();
			objData.pivot = caslData->pivot;
			auto isHook = false;
			if (hookFunc != nullptr) {
				//hook関数が指定されているのでhook関数を呼び出す。
				isHook = hookFunc(objData);
				if (!isHook)
				{
					AddMapChip2DRender(&objData);
				}
			}
			else
			{
				AddMapChip2DRender(&objData);
			}
		}
	}

	void Level2DRender::Update() const
	{
		for (auto& mapChip : m_mapChip2DRenderPtrList)
		{
			mapChip.get()->Update();
		}
	}

	void Level2DRender::Draw(RenderContext& renderContext) const
	{
		for (auto& mapChip : m_mapChip2DRenderPtrList)
		{
			mapChip.get()->Draw(renderContext);
		}
	}
}