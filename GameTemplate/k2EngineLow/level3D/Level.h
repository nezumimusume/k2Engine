/*!
 *@brief	レベル。
 */

#pragma once
#include "level3D/TklFile.h"

namespace nsK2EngineLow {
	class MapChip;
	/// <summary>
	/// レベルオブジェクト。
	/// </summary>
	struct LevelObjectData : public Noncopyable {
		Vector3 position;		//座標。
		Quaternion rotation;	//回転。
		Vector3 scale;			//拡大率。
		const wchar_t* name;	//名前。
		int number;
		/// <summary>
		/// 引数で渡したオブジェクト名のオブジェクトか調べる。
		/// </summary>
		/// <param name="objName">調べる名前。</param>
		/// <returns>名前が同じ場合にtrueを返します。</returns>
		bool EqualObjectName(const wchar_t* objName)
		{
			return wcscmp(objName, name) == 0;
		}
		/// <summary>
		/// 名前が前方一致するか調べる。
		/// </summary>
		/// <param name="n"></param>
		/// <returns></returns>
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

	/// <summary>
	/// レベル。
	/// </summary>
	class Level {
	private:
		using MapChipPtr = std::shared_ptr<MapChip>;
	public:
		~Level();
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
		/// モデルを描画。
		/// </summary>
		/// <param name="rc">レンダーコンテキスト。</param>
		void Draw(RenderContext& rc);
	private:
		/// <summary>
		/// マップチップを作成。
		/// </summary>
		/// <param name="objData">LevelObjectData。</param>
		void CreateMapChip(const LevelObjectData& objData, const char* filePath);
		/// <summary>
		/// Tklファイルの行列を変換する。
		/// </summary>
		void MatrixTklToLevel();
	private:
		using BonePtr = std::unique_ptr<Bone>;						//ボーンPtr。
		std::vector<BonePtr> m_bonelist;							//ボーンのリスト。
		std::unique_ptr<Matrix[]> m_matrixlist;						//行列のリスト。
		std::vector<MapChipPtr> m_mapChipPtrs;						//マップチップの可変長配列。
		TklFile m_tklFile;											//Tklファイル。


	};
}