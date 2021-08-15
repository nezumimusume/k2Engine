/*!
 *@brief	マップチップ
 */

#pragma once

namespace nsK2Engine {
	struct LevelObjectData;

	struct MapChipData
	{
		Vector3				position;				//座標。
		Vector3				scale;					//大きさ。
		Quaternion			rotation;				//回転。
		//std::unique_ptr<PhysicsStaticObject> physicsStaticObject;	//静的物理オブジェクト。
	};

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
		/// マップチップデータを追加。
		/// </summary>
		/// <param name="objData">マップチップデータ。</param>
		void AddMapChipData(const LevelObjectData& objData);
		/// <summary>
		/// 初期化。
		/// </summary>
		void Init();
		/// <summary>
		/// モデルを更新。
		/// </summary>
		void Update();
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
		ModelRender											m_modelRender;					//モデルレンダー。
		std::vector<MapChipData>							m_mapChipDataVector;			//マップチップデータのリスト。
		std::unique_ptr<const char*>						m_filePath;						//tkmファイルパス。
		std::vector<std::unique_ptr<PhysicsStaticObject>>	m_physicsStaticObjectPtrVector;	//PhysicsStaticObjectのスマートポインタの配列。
		int													m_mapChipDataNum;				//マップチップデータの数。
	};
}