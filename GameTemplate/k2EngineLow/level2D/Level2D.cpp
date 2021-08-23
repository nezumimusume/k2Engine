/*!
 *@brief	レベル。
 */

#include "k2EngineLowPreCompile.h"
#include "Level2D.h"
#include "CaslFile.h"
#include "MapChip2D.h"

namespace nsK2EngineLow {
	void Level2D::AddMapChip2D(CaslData* caslData)
	{
		auto mapChip2DPtr = std::make_unique<MapChip2D>();
		mapChip2DPtr.get()->Init(caslData);
		//std::unique_ptrを移動させる時はstd::move()を使う。
		m_mapChip2DPtrList.push_back(std::move(mapChip2DPtr));
	}

	void Level2D::Init(
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
					AddMapChip2D(caslData);
				}
			}
			else
			{
				AddMapChip2D(caslData);
			}
		}

	}

	void Level2D::Update() const
	{
		for (auto& mapChip : m_mapChip2DPtrList)
		{
			mapChip.get()->Update();
		}
	}

	void Level2D::Draw(RenderContext& renderContext) const
	{
		for (auto& mapChip : m_mapChip2DPtrList)
		{
			mapChip.get()->Draw(renderContext);
		}
	}
}