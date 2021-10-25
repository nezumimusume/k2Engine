#pragma once

#include <string>

/// <summary>
/// nvmExporterのコア
/// </summary>
class nvmExporterCore
{
public:
	/// <summary>
	/// エクスポート
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	void Execute(const std::wstring& filePath);
};

