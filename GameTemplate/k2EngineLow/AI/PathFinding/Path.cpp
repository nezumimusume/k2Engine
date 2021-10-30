#include "k2EngineLowPreCompile.h"
#include "AI/PathFinding/Path.h"

namespace nsK2EngineLow {
	namespace nsAI {
		void Path::Build()
		{
			m_sectionArray.resize(m_pointArray.size() - 1);
			// セクションを構築していく。
			for (int pointNo = 0; pointNo < m_pointArray.size() - 1; pointNo++) {
				auto& section = m_sectionArray.at(pointNo);
				section.startPos = m_pointArray.at(pointNo);
				section.endPos = m_pointArray.at(pointNo+1);
				section.direction = section.endPos - section.startPos;
				section.length = section.direction.Length();
				section.direction.Normalize();
			}
		}
	}
}


