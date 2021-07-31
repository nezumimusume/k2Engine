/*!
 *@brief	マップチップ
 */

#pragma once




struct LevelObjectData;

/// <summary>
/// マップチップレンダー。
/// </summary>
class MapChipRender : public Noncopyable
{
public:
	MapChipRender(const LevelObjectData& objData, const char* filePath);
	~MapChipRender()
	{
	}
	/// <summary>
	/// モデルを描画。
	/// </summary>
	/// <param name="rc">レンダーコンテキスト。</param>
	void Draw(RenderContext& rc);
	/// <summary>
	/// モデルレンダラーを取得。
	/// </summary>
	/// <returns></returns>
	ModelRender& GetSkinModelRender()
	{
		return m_modelRender;
	}
private:
	ModelRender			m_modelRender;					//モデルレンダー。
	PhysicsStaticObject m_physicsStaticObject;			//静的物理オブジェクト。
};
