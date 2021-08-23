#include "k2EngineLowPreCompile.h"
#include "MapChip2D.h"
#include "CaslFile.h"

namespace nsK2EngineLow {
	void MapChip2D::Init(CaslData* caslData)
	{
		SpriteInitData data;
		//�����A�c���B
		data.m_width = caslData->width;
		data.m_height = caslData->height;
		//�摜��ǂݍ��ށB
		data.m_ddsFilePath[0] = caslData->ddsFilePath.get();
		data.m_fxFilePath = "Assets/shader/sprite.fx";
		m_sprite.Init(data);
		//���W�B
		m_position.x = caslData->position.x;
		m_position.y = caslData->position.y;
		//�傫���B
		m_scale.x = caslData->scale.x;
		m_scale.y = caslData->scale.y;
		//�s�{�b�g�B
		m_pivot = caslData->pivot;
	}
}

