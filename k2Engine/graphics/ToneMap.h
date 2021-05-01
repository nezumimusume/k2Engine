#pragma once
class ToneMap
{
public:
	ToneMap();
	~ToneMap();
	/// <summary>
   /// 描画。
   /// </summary>
   /// <param name="rc">レンダ―コンテキスト。</param>
   /// <param name="mainRenderTarget">メインレンダ―ターゲット。</param>
	void Render(RenderContext& rc, RenderTarget& mainRenderTarget);
	/// <summary>
	/// 初期化。
	/// </summary>
	void Init(RenderTarget& mainRenderTarget);
private:
	struct TonemapBuffer {
		float scaler = 35.0f;
		float gammaCorrection = 2.2f;
	};

	std::vector<std::unique_ptr<RenderTarget>> m_renderTargetVector;		//平均輝度を求めるためのレンダ―ターゲット。
	std::vector<std::unique_ptr<Sprite>> m_spriteVector;
	Sprite m_finalSprite;
	TonemapBuffer m_tonemapBuffer;
};

