/*!
 *@brief	マップチップのレンダラー。
 */

#pragma once

//#include "tkEngine/physics/tkPhysicsStaticObject.h"

namespace tkEngine{
	struct LevelObjectData;
	/*!
	 *@brief	マップチップレンダラー。
	 */
	class CMapChipRender : public IGameObject{
	public:
		CMapChipRender();
		~CMapChipRender();
		/// <summary>
		/// 毎フレーム呼び出される更新処理。
		/// </summary>
		void Update() override;
		/// <summary>
		/// 描画するオブジェクトの数を追加。
		/// </summary>
		void AddRenderObject( const LevelObjectData& objData )
		{
			m_renderObjDatas.push_back(objData);
		}
		/// <summary>
		/// 全ての描画オブジェクトの追加が終わった後で呼び出す必要がある初期化処理。
		/// </summary>
		void InitAfterAddAllRenderObjects();
		/// <summary>
		/// スキンモデルレンダラーを取得。
		/// </summary>
		/// <returns></returns>
		ModelRender& GetSkinModelRender()
		{
			return m_modelRender;
		}
		/// <summary>
		/// 描画オブジェクトデータにクエリを行う。
		/// </summary>
		/// <param name="queryFunc">クエリ関数</param>
		void QueryRenderObjDatas(std::function<void(const LevelObjectData& objData)> queryFunc) 
		{
			for (const auto& renderObjData : m_renderObjDatas) {
				queryFunc(renderObjData);
			}
		}
	private:
		int m_numRenderObject = 0;
		std::vector<LevelObjectData> m_renderObjDatas;	//描画するオブジェクトのデータの配列。
		ModelRender m_modelRender;	//!<モデルレンダラー。
	};
}
