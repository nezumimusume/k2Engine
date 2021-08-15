/*!
 *@brief	レベル。
 */

#pragma once
#include "level3D/TklFile.h"

namespace nsK2Engine {
	class MapChipRender;
	/*!
	* @brief	オブジェクト名。
	*/
	struct LevelObjectData {
		Vector3 position;		//<座標。
		Quaternion rotation;	//!<回転。
		Vector3 scale;			//!<拡大率。
		const wchar_t* name;	//!<名前。
		int number = 0;
		/*!
		* @brief	引数で渡したオブジェクト名のオブジェクトか調べる。
		*@param[in]	objName		調べる名前。
		*@return	名前が同じ場合にtrueを返します。
		*/
		bool EqualObjectName(const wchar_t* objName)
		{
			return wcscmp(objName, name) == 0;
		}
		/*!
		* @brief	名前が前方一致するか調べる。
		*/
		bool ForwardMatchName(const wchar_t* n)
		{
			auto len = wcslen(n);
			auto namelen = wcslen(name);
			if (len > namelen) {
				//名前が長い。不一致。
				return false;
			}
			return wcsncmp(n, name, len) == 0;
		}
	};

	/*!
	*@brief	レベル。
	*/
	class LevelRender : public Noncopyable {
	private:
		using MapChipRenderPtr = std::shared_ptr<MapChipRender>;
	public:
		~LevelRender();
		/*!
		 * @brief	レベルを初期化。
		 *@param[in]	levelDataFilePath		tklファイルのファイルパス。
		 *@param[in] hookFunc				オブジェクトを作成する時の処理をフックするための関数オブジェクト。
		 *   フックしないならnullptrを指定すればよい、
		 * この関数オブジェクトがfalseを返すと、オブジェクトの情報から、
		 * 静的オブジェクトのMapChipクラスのインスタンスが生成されます。
		 * オブジェクトの名前などで、ドアやジャンプ台、アイテムなどの特殊なクラスのインスタンスを生成したときに、
		 * デフォルトで作成されるMapChipクラスのインスタンスが不要な場合はtrueを返してください。
		 * 例えば、フック関数の中で、渡されたオブジェクトデータの名前のモデルを描画するクラスのインスタンスを
		 * 生成したときに、falseを返してしまうと、同じモデルが二つ描画されることになります。
		 */
		void Init(const char* filePath, std::function<bool(LevelObjectData& objData)> hookFunc);
		/// <summary>
		/// 更新処理。
		/// </summary>
		void Update();
		/// <summary>
		/// モデルを描画。
		/// </summary>
		/// <param name="rc">レンダーコンテキスト。</param>
		void Draw(RenderContext& rc);
	private:
		/// <summary>
		/// マップチップを作成。
		/// </summary>
		/// <param name="objData">LevelObjectData。</param>
		void CreateMapChipRender(const LevelObjectData& objData, const char* filePath);
		/// <summary>
		/// Tklファイルの行列を変換する。
		/// </summary>
		void MatrixTklToLevel();
	private:
		using BonePtr = std::unique_ptr<Bone>;								//ボーンPtr。
		std::vector<BonePtr> m_bonelist;									//ボーンのリスト。
		std::unique_ptr<Matrix[]> m_matrixlist;								//行列のリスト。
		std::map<std::string, MapChipRenderPtr> m_mapChipRenderPtrs;		//マップチップの可変長配列。
		TklFile m_tklFile;													//Tklファイル。
	};
}