#pragma once

#include <string>

/// <summary>
/// nvmExporter�̃R�A
/// </summary>
class nvmExporterCore
{
public:
	/// <summary>
	/// �G�N�X�|�[�g
	/// </summary>
	/// <param name="filePath">�t�@�C���p�X</param>
	bool Execute(const std::wstring& filePath);
};

