#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// tklファイルを読み込む。
	/// </summary>
	class TklFile : public Noncopyable
	{
	public:
		//オブジェクトデーター。
		struct SObject
		{
			std::unique_ptr<char[]> name;	//骨の名前。
			int parentNo;					//親の番号。
			float bindPose[4][3];			//バインドポーズ。
			float invBindPose[4][3];		//バインドポーズの逆数。
			int no;

			//スクリプトの方がまだ未実装。
			bool shadowcasterflag = false;
			bool shadowreceiverflag = false;
			std::vector<int> intData;
			std::vector<float> floatData;
			std::vector<char*> charData;
			std::vector<Vector3> Vector3Data;
		};
		/// <summary>
		/// ロード。
		/// </summary>
		/// <param name="filepath">ファイルパス。</param>
		void Load(const char* filepath);
		//クエリ関数。
		void QuaryObject(std::function<void(SObject& obj)> query)
		{
			for (auto& obj : m_objectlist) {
				query(obj);
			}
		}
	private:
		//バージョンと骨の数
		int m_version = 0;
		int m_objectCout = 0;
		std::vector<SObject> m_objectlist;
	};
}