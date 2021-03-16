/*!
 *@brief	波形データバンク。
 */

#include "k2EnginePreCompile.h"
#include "WaveFile.h"
#include "WaveFileBank.h"


	WaveFileBank::WaveFileBank()
	{
	}
	WaveFileBank::~WaveFileBank()
	{
		ReleaseAll();
	}
	void WaveFileBank::Release(int groupID)
	{
		for (auto waveFile : m_waveFileMap[groupID]) {
			waveFile.second->Release();
		}
		m_waveFileMap[groupID].clear();
	}
	void WaveFileBank::UnregistWaveFile(int groupID, WaveFilePtr waveFile)
	{
		MY_ASSERT(groupID < MAX_GROUP, "groupID is invalid");
		m_waveFileMap->erase(waveFile->GetFilePathHash());
	}
	void WaveFileBank::RegistWaveFile(int groupID, WaveFilePtr waveFile)
	{
		MY_ASSERT(groupID < MAX_GROUP, "groupID is invalid");
		m_waveFileMap[groupID].insert({ waveFile->GetFilePathHash(), waveFile });
	}
	WaveFilePtr WaveFileBank::FindWaveFile(int groupID, const wchar_t* filePath)
	{
		MY_ASSERT(groupID < MAX_GROUP, "groupID is invalid");
		auto value = m_waveFileMap[groupID].find(CUtil::MakeHash(filePath));
		if (value != m_waveFileMap[groupID].end()) {
			return value->second;
		}
		return WaveFilePtr();
	}
	WaveFilePtr WaveFileBank::FindWaveFile(int groupID, const WNameKey& nameKey)
	{
		MY_ASSERT(groupID < MAX_GROUP, "groupID is invalid");
		auto value = m_waveFileMap[groupID].find(nameKey.GetHashCode());
		if (value != m_waveFileMap[groupID].end()) {
			return value->second;
		}
		return WaveFilePtr();
	}
