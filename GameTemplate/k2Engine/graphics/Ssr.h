#pragma once

/// <summary>
/// スクリーンスペースリフレクション
/// </summary>
class Ssr {
public:
	/// <summary>
	/// 初期化。
	/// </summary>
	void Init();
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="rc"></param>
	void Render(RenderContext& rc);
};