#include "stdafx.h"
#include "Game.h"

Game::Game()
{

}

Game::~Game()
{

}

bool Game::Start()
{
	//モデルを読み込む。
	m_modelRender.Init("Assets/modelData/unityChan.tkm");

	//レベルを構築する。
	m_level2DRender.Init("Assets/level2D/sample.casl", [&](Level2DObjectData& objData) {
		//名前が一致していたら。
		if (objData.EqualObjectName("k2SLEditor_red") == true) {


			//return falseにすると、Level2D側で画像が読み込まれます。
			return false;
		}
		else if (objData.EqualObjectName("k2SLEditor_blue") == true) {

			//レベルのデータを使用して画像を読み込む。
			m_spriteRender.Init(objData.ddsFilePath, objData.width, objData.height);
			m_spriteRender.SetScale(objData.scale);
			m_spriteRender.SetPosition(objData.position);
			m_spriteRender.SetPivot(objData.pivot);
			//return trueにすると、Level2D側で画像が読み込まれます。
			//自身で画像を読み込みたい時は必ずtrueにしてください。
			return true;
		}
		return false;
		});

	return true;
}

void Game::Update()
{
	//モデルの更新。
	m_modelRender.Update();

	//レベル2D側で読み込んだ画像の更新。
	m_level2DRender.Update();

	//画像の更新。
	m_spriteRender.Update();
}

void Game::Render(RenderContext& rc)
{
	//モデルの描画。
	m_modelRender.Draw(rc);

	//レベル2D側で読み込んだ画像の描画処理。
	m_level2DRender.Draw(rc);

	//画像の描画。
	m_spriteRender.Draw(rc);
}