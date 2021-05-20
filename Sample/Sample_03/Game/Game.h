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
	enum EnAnimationClip {
		enAnimationClip_idle,	//待機アニメーション。
		enAnimationClip_run,	//走りアニメーション。
		enAnimationClip_walk,	//歩きアニメーション。
		enAnimationClip_Num,	//アニメーションクリップの数。
	};
	ModelRender m_modelRender;
	AnimationClip m_animClips[enAnimationClip_Num];		//アニメーションクリップ。
};

