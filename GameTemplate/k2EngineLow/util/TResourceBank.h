#pragma once

#include <memory>

namespace nsK2EngineLow {
	template<class TResource>
	class TResourceBank : public Noncopyable {
	public:
		TResource* Get(const char* filePath)
		{
			auto it = m_resourceMap.find(filePath);
			if (it != m_resourceMap.end()) {
				//�o���N�ɓo�^����Ă���B
				return it->second.get();
			}
			return nullptr;
		}
		void Regist(const char* filePath, TResource* resource)
		{
			auto it = m_resourceMap.find(filePath);
			if (it == m_resourceMap.end()) {
				//���o�^�B
				m_resourceMap.insert(
					std::pair< std::string, TResourcePtr>(filePath, resource)
				);
			}
		}
	private:
		using TResourcePtr = std::unique_ptr<TResource>;
		std::map<std::string, TResourcePtr> m_resourceMap;
	};
}