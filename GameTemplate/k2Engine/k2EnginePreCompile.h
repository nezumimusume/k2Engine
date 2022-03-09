#pragma once
#define _CRT_SECURE_NO_WARNINGS


// #define COPY_RAYTRACING_FRAMEBUFFER		// 有効でレイトレの結果をフレームバッファに書き出す。

#include "k2EngineLowPreCompile.h"
using namespace nsK2EngineLow;
#include "collision/CollisionObject.h"
#include "graphics/RenderingEngine.h"
#include "k2Engine.h"
#include "geometry/AABB.h"
#include "graphics/IRenderer.h"
#include "graphics/ModelRender.h"
#include "graphics/SpriteRender.h"
#include "graphics/FontRender.h"
#include "graphics/light/SceneLight.h"
#include "nature/SkyCube.h"
#include "Level3DRender/LevelRender.h"
#include "graphics/light/SpotLight.h"
#include "graphics/light/PointLight.h"
#include "graphics/light/VolumeSpotLight.h"
#include "graphics/light/VolumePointLight.h"
