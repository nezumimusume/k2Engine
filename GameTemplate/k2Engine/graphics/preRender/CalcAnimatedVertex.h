#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// アニメーション済み頂点計算処理。
	/// </summary>
	/// <remark>
	/// コンピュートシェーダーでアニメーション済み頂点を計算します。
	/// </remark>
	class CalcAnimatedVertex : public Noncopyable {
	public:
		/// <summary>
		/// 初期化。
		/// </summary>
		void Init();
		/// <summary>
		/// 実行。
		/// </summary>
		void Execute(RenderContext& rc);
	};
}