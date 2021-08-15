#pragma once

namespace nsK2Engine {
	// シーンのディレクションライトの最大数
	const int MAX_DIRECTIONAL_LIGHT = 4;
	// シーンのポイントライトの最大数。
	const int MAX_POINT_LIGHT = 1000;


	// TBR用のパラメータ。
	const int TILE_WIDTH = 16;  // タイルの幅
	const int TILE_HEIGHT = 16; // タイルの高さ
	const int NUM_TILE = (FRAME_BUFFER_W / TILE_WIDTH) * (FRAME_BUFFER_H / TILE_HEIGHT); // タイルの数

	// シャドウマップ
	enum {
		SHADOW_MAP_AREA_NEAR,		//近影範囲のシャドウマップ。
		SHADOW_MAP_AREA_MIDDLE,		//中影範囲のシャドウマップ。
		SHADOW_MAP_AREA_FAR,		//遠影範囲のシャドウマップ。
		NUM_SHADOW_MAP
	};
}
