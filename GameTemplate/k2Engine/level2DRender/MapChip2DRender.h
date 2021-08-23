#pragma once

namespace nsK2Engine {
	struct Level2DObjectData;
	/// <summary>
	/// マップチップ2D。
	/// </summary>
	class MapChip2DRender : public Noncopyable
	{
	public:
		/// <summary>
		/// 初期化。
		/// </summary>
		/// <param name="objData">レベル2Dデータ。</param>
		void Init(Level2DObjectData* objData);
		/// <summary>
		/// 更新処理。
		/// </summary>
		void Update()
		{
			m_spriteRender.Update();
		}
		/// <summary>
		/// スプライトを描画する。
		/// </summary>
		/// <param name="renderContext">レンダーコンテキスト。</param>
		void Draw(RenderContext& renderContext)
		{
			m_spriteRender.Draw(renderContext);
		}
	private:
		SpriteRender m_spriteRender;			//スプライト。
	};
}

