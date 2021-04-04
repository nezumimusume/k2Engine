#include "k2EnginePreCompile.h"
#include "SpriteRender.h"

void SpriteRender::Init(const char* filePath, const float w, const float h)
{
	SpriteInitData initData;
	//DDSファイル(画像データ)のファイルパスを指定する。
	initData.m_ddsFilePath[0] = filePath;
	//Sprite表示用のシェーダーのファイルパスを指定する。
	initData.m_fxFilePath = "Assets/shader/sprite.fx";
	//スプライトの幅と高さを指定する。
	initData.m_width = w;
	initData.m_height = h;
	//Sprite初期化オブジェクトを使用して、Spriteを初期化する。
	m_sprite.Init(initData);
}
