#pragma once
class ToneMap : public Noncopyable
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
	static const int NUM_RENDER_TARGETS_DRAW_ONE_FRAME = 2;		//1フレームに描画する、平均輝度を求めるためのレンダーターゲットの最大数。
private:
	/// <summary>
	/// 平均輝度を求めるための描画。
	/// </summary>
	/// <param name="rc">レンダーコンテキスト。</param>
	/// <param name="numberRenderTarget">レンダーターゲットの番号。</param>
	void RenderToLuma(RenderContext& rc,int numberRenderTarget)
	{
		//描画予定のレンダーターゲットはPRESENTからRENDERTARGETへ。
		rc.WaitUntilToPossibleSetRenderTarget(*m_renderTargetVector[numberRenderTarget].get());
		//レンダリングターゲットを設定
		rc.SetRenderTargetAndViewport(*m_renderTargetVector[numberRenderTarget].get());
		//描画。
		m_spriteVector[numberRenderTarget].get()->Draw(rc);
		//次のレンダーターゲット描画の際に。
		//シェーダーリソースビューとして使用するので、RENDERTARGETからPRESENTへ。
		rc.WaitUntilFinishDrawingToRenderTarget(*m_renderTargetVector[numberRenderTarget].get());
	}
	struct TonemapBuffer {
		float scaler = 7.0f;
		float gammaCorrection = 2.0f;
	};

	std::vector<std::unique_ptr<RenderTarget>> m_renderTargetVector;		//平均輝度を求めるためのレンダ―ターゲット。
	std::vector<std::unique_ptr<Sprite>> m_spriteVector;
	Sprite m_finalSprite;
	TonemapBuffer m_tonemapBuffer;
	int m_numberCalcRenderTarget = -1;		
}; 

