#pragma once

/// <summary>
/// レンダーラーの基底クラス。
/// </summary>
class IRenderer {
public:
	/// <summary>
	/// シャドウマップへの描画パスから呼ばれる処理。
	/// </summary>
	/// <param name="rc">レンダリングコンテキスト</param>
	/// <param name="ligNo">ライト番号</param>
	/// <param name="lvpMatrix">ライトビュープロジェクション行列</param>
	virtual void OnRenderShadowMap(
		RenderContext& rc, 
		int ligNo, 
		int shadowMapNo,
		const Matrix& lvpMatrix
	)
	{

	}
	/// <summary>
	/// ZPrepassから呼ばれる処理。
	/// </summary>
	/// <param name="rc"></param>
	virtual void OnZPrepass(RenderContext& rc)
	{

	}
	/// <summary>
	/// G-Buffer描画パスから呼ばれる処理。
	/// </summary>
	virtual void OnRenderToGBuffer(RenderContext& rc)
	{

	}
	/// <summary>
	/// フォワードレンダーパスから呼ばれる処理。
	/// </summary>
	virtual void OnForwardRender(RenderContext& rc)
	{

	}
	/// <summary>
	/// 2D描画パスから呼ばれる処理。
	/// </summary>
	/// <param name="rc"></param>
	virtual void OnRender2D(RenderContext& rc)
	{

	}

};