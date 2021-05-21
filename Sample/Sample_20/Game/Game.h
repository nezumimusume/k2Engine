#pragma once
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
	//////////////////////////////////////
	// メンバ変数。
	//////////////////////////////////////
	ModelRender m_modelRender;		//モデルレンダ―。
	Quaternion m_rotation;			//クォータニオン。
	int m_rotateMode = 0;			//0は自動回転、1はスティックで回転。
};

