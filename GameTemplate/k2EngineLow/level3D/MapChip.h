/// <summary>
/// マップチップ。
/// </summary>
#pragma once

namespace nsK2EngineLow {
	struct LevelObjectData;

	/// <summary>
	/// マップチップ。
	/// </summary>
	class MapChip : public Noncopyable
	{
	public:
		MapChip(const LevelObjectData& objData, const char* filePath);
		~MapChip()
		{
		}
		/// <summary>
		/// モデルを描画。
		/// </summary>
		/// <param name="rc">レンダーコンテキスト。</param>
		void Draw(RenderContext& rc);
		/// <summary>
		/// モデルを取得。
		/// </summary>
		/// <returns></returns>
		Model& GetSkinModel()
		{
			return m_model;
		}
	private:
		Model				m_model;						//モデルレンダー。
		PhysicsStaticObject m_physicsStaticObject;			//静的物理オブジェクト。
	};
}