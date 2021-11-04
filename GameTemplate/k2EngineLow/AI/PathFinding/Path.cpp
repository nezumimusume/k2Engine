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
		Vector3 Path::Move(
			Vector3 pos, 
			float moveSpeed, 
			bool& isEnd,
			PhysicsWorld* physicsWorld
		){
			if (m_sectionArray.empty() 
				|| m_sectionNo >= m_sectionArray.size() 
			) {
				// パスが構築されていない
				return pos;
			}
			SSection& currentSection = m_sectionArray.at(m_sectionNo);
			// セクションの終点に向かうベクトルを計算する。
			Vector3 toEnd = currentSection.endPos - pos;
			toEnd.Normalize();
			pos += toEnd * moveSpeed;

			Vector3 toEnd2 = currentSection.endPos - pos;
			toEnd2.Normalize();

			if (toEnd.Dot(toEnd2) <= 0.0f) {
				// 向きが変わったので終点を超えた。
				pos = currentSection.endPos;
				if (m_sectionNo == m_sectionArray.size() - 1) {
					// 終点
					isEnd = true;
				}
				else {
					m_sectionNo++;
				}
			}
			if (physicsWorld) {
				Vector3 rayStart = pos;
				// ちょっと上。
				rayStart.y += 1.0f;
				Vector3 rayEnd = rayStart;
				rayEnd.y -= 1000.0f;
				Vector3 hitPos;
				if (physicsWorld->RayTest(rayStart, rayEnd, hitPos)) {
					pos = hitPos;
				}
			}
			return pos;
		}
	}
}


