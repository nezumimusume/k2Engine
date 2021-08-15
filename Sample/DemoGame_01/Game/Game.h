#pragma once




//クラス宣言。
class Player;
class GameCamera;
class Background;
class Lever;
class Door;
class Fade;

/// <summary>
/// ゲーム。
/// </summary>
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
	/// <summary>
	/// ゲームクリアーを通知する。
	/// </summary>
	void NotifyGameClear();
	/// <summary>
	/// ゲームオーバーを通知する。
	/// </summary>
	void NotifyGameOver();
	/// <summary>
	/// エネミーを全滅させた？
	/// </summary>
	/// <returns>エネミーが全滅していたらtrue。</returns>
	const bool IsWannihilationEnemy() const
	{
		return m_numEnemy == m_numDefeatedEnemy;
	}
	/// <summary>
	/// 倒された敵の数を+1する。
	/// </summary>
	void AddDefeatedEnemyNumber()
	{
		m_numDefeatedEnemy++;
	}
	//////////////////////////////////////
	// メンバ変数。
	//////////////////////////////////////
private:
	LevelRender				m_levelRender;					//レベル。
	Player*					m_player = nullptr;				//プレイヤー。
	GameCamera*				m_gameCamera = nullptr;			//カメラ。
	SkyCube*				m_skyCube = nullptr;			//空。
	Background*				m_background = nullptr;			//背景。
	std::vector<Lever*>		m_leverVector;					//レバーの配列。
	std::vector<Door*>		m_doorVector;					//ドアの配列。
	SoundSource*			m_bgm = nullptr;				//ゲーム中のBGM。
	Fade*					m_fade = nullptr;				//フェード。
	bool					m_isWaitFadeout = false;
	int						m_numEnemy = 0;					//エネミーの数。
	int						m_numDefeatedEnemy = 0;			//倒されたエネミーの数。
};

