/////////////////////////////////////////////////////
// PBRライティング関係の定数をまとめたヘッダーファイル
/////////////////////////////////////////////////////

#ifndef _PBRLIGHTING_CONST_H_
#define _PBRLIGHTING_CONST_H_

static const int MAX_POINT_LIGHT = 256;         // ポイントライトの最大数。
static const int MAX_SPOT_LIGHT = 256;          // スポットライトの最大数。
static const int NUM_DIRECTIONAL_LIGHT = 4;     // ディレクションライトの本数
static const float PI = 3.1415926f;         // π

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

#endif // _PBRLIGHTING_CONST_H_