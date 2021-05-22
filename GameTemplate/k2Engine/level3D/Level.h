/*!
 *@brief	レベル。
 */

#pragma once


class MapChip;
/*!
* @brief	オブジェクト名。
*/
struct LevelObjectData {
	Vector3 position;		//<座標。
	Quaternion rotation;	//!<回転。
	Vector3 scale;			//!<拡大率。
	char* name;	//!<名前。
	/*!
	* @brief	引数で渡したオブジェクト名のオブジェクトか調べる。
	*@param[in]	objName		調べる名前。
	*@return	名前が同じ場合にtrueを返します。
	*/
	bool EqualObjectName(const char* objName)
	{
		return strcmp(objName, name) == 0;
	}
	/*!
	* @brief	名前が前方一致するか調べる。
	*/
	bool ForwardMatchName(const char* n)
	{
		auto len = strlen(n);
		auto namelen = strlen(name);
		if (len > namelen) {
			//名前が長い。不一致。
			return false;
		}
		return strncmp(n, name, len) == 0;
	}
};
	/*!
	 *@brief	レベル。
	 */
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
	void CreateMapChip(const LevelObjectData& objData);
private:
	std::vector<MapChipPtr> m_mapChipPtrs;			//マップチップの可変長配列。

};
