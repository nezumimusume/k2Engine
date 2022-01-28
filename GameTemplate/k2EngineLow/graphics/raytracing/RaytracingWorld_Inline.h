#pragma once



namespace nsK2EngineLow {
namespace raytracing {
		inline void World::QueryInstances(int bufferNo, std::function<void(Instance&)> queryFunc) const
		{
			for (auto& instance : m_instances[bufferNo]) {
				queryFunc(*instance);
			}
		}
		inline int World::GetNumInstance() const
		{
			return static_cast<int>(m_instances[g_graphicsEngine->GetBackBufferIndex()].size());
		}
		
		inline const BLASBuffer& World::GetBLASBuffer()
		{
			return m_blasBuffer[g_graphicsEngine->GetBackBufferIndex()];
		}
		
		inline TLASBuffer& World::GetTLASBuffer()
		{
			return m_topLevelASBuffers[g_graphicsEngine->GetBackBufferIndex()];
		}
}//namespace raytracing
}

