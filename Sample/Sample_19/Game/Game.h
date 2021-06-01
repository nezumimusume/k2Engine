#pragma once

//クラス宣言。
class Player;
class Background;
class GameCamera;
class Box;

class Game : public IGameObject
{
public:
	//////////////////////////////////////
	// メンバ関数。
	//////////////////////////////////////
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	////////////////////////////////////
	// メンバ変数とか
	////////////////////////////////////
	Player*			m_player = nullptr;				//プレイヤー。
	Background*		m_background = nullptr;			//背景。
	GameCamera*		m_gameCamera = nullptr;			//ゲームカメラ。
	Box*			m_box = nullptr;				//ボックス。
	FontRender		m_fontRender;					//フォント。
	bool			m_isHit = false;				//キャラコンとゴーストが衝突した？
};

