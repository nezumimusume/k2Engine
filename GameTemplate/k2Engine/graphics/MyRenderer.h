#pragma once

// ディファードライティングで使用されるディレクションライトの数
const int NUM_DEFERRED_LIGHTING_DIRECTIONAL_LIGHT = 4;
// シャドウマップ
enum {
	SHADOW_MAP_AREA_NEAR,		//近影範囲のシャドウマップ。
	SHADOW_MAP_AREA_MIDDLE,		//中影範囲のシャドウマップ。
	SHADOW_MAP_AREA_FAR,		//遠影範囲のシャドウマップ。
	NUM_SHADOW_MAP
};
