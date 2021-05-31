#include "k2EnginePreCompile.h"
#include "MapChip2DRender.h"
#include "level2DRender/Level2DRender.h"

void MapChip2DRender::Init(Level2DObjectData* objData)
{
	//‰Šú‰»B
	m_spriteRender.Init(objData->ddsFilePath, objData->width, objData->height);
	m_spriteRender.SetPosition(objData->position);
	m_spriteRender.SetScale(objData->scale);
}


