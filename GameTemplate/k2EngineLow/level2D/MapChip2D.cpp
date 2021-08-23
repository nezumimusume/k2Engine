#include "k2EngineLowPreCompile.h"
#include "MapChip2D.h"
#include "CaslFile.h"

namespace nsK2EngineLow {
	void MapChip2D::Init(CaslData* caslData)
	{
		SpriteInitData data;
		//横幅、縦幅。
		data.m_width = caslData->width;
		data.m_height = caslData->height;
		//画像を読み込む。
		data.m_ddsFilePath[0] = caslData->ddsFilePath.get();
		data.m_fxFilePath = "Assets/shader/sprite.fx";
		m_sprite.Init(data);
		//座標。
		m_position.x = caslData->position.x;
		m_position.y = caslData->position.y;
		//大きさ。
		m_scale.x = caslData->scale.x;
		m_scale.y = caslData->scale.y;
		//ピボット。
		m_pivot = caslData->pivot;
	}
}

