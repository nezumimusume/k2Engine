/*!
 * @brief	ユーティリティ関数。
 */
#pragma once

namespace nsK2EngineLow{
	
	/*!
	 * @brief	文字列から32bitのハッシュ値を作成。
	 */
	static int MakeHash( const char* string )
	{
		if (string == nullptr) {
			return 0;
		}
		int hash = 0;
		int l = (int)strlen(string);
		for( int i = 0; i < l; i++ ){
			hash = hash * 37 + string[i];
		}
		return hash;
	}

	/*!
	* @brief	文字列から32bitのハッシュ値を作成。
	*/
	static int MakeHash(const wchar_t* string)
	{
		if (string == nullptr) {
			return 0;
		}
		int hash = 0;
		int l = (int)wcslen(string);
		for (int i = 0; i < l; i++) {
			hash = hash * 37 + string[i];
		}
		return hash;
	}
}
