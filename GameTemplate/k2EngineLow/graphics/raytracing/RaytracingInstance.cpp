#include "k2EngineLowPreCompile.h"
#include "RaytracingInstance.h"

namespace nsK2EngineLow {
	namespace raytracing {
		void Instance::Release()
		{
			if (m_blasStructuredBuffers.pScratch) {
				ReleaseD3D12Object(m_blasStructuredBuffers.pScratch);
			}
			if (m_blasStructuredBuffers.pResult) {
				ReleaseD3D12Object(m_blasStructuredBuffers.pResult);
			}
		}
	}
}
