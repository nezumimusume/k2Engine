#include "k2EnginePreCompile.h"
#include "MapChip2DRender.h"
#include "level2D/CaslFile.h"

void MapChip2DRender::Init(CaslData* caslData)
{
	//‰Šú‰»B
	m_spriteRender.Init(caslData->ddsFilePath.get(), caslData->width, caslData->height);
	m_spriteRender.SetPosition(Vector3(caslData->position.x, caslData->position.y,1.0f));
	m_spriteRender.SetScale(Vector3(caslData->scale.x, caslData->scale.y, 1.0f));

}


