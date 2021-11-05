#include "k2EngineLowPreCompile.h"
#include "AI/PathFinding/Path.h"

namespace nsK2EngineLow {
	namespace nsAI {
		void Path::Build()
		{
			m_sectionArray.resize(m_pointArray.size() - 1);
			// �Z�N�V�������\�z���Ă����B
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
				// �p�X���\�z����Ă��Ȃ�
				return pos;
			}
			SSection& currentSection = m_sectionArray.at(m_sectionNo);
			// �Z�N�V�����̏I�_�Ɍ������x�N�g�����v�Z����B
			Vector3 toEnd = currentSection.endPos - pos;
			toEnd.Normalize();
			pos += toEnd * moveSpeed;

			Vector3 toEnd2 = currentSection.endPos - pos;
			toEnd2.Normalize();

			if (toEnd.Dot(toEnd2) <= 0.0f) {
				// �������ς�����̂ŏI�_�𒴂����B
				pos = currentSection.endPos;
				if (m_sectionNo == m_sectionArray.size() - 1) {
					// �I�_
					isEnd = true;
				}
				else {
					m_sectionNo++;
				}
			}
			if (physicsWorld) {
				Vector3 rayStart = pos;
				// ������Ə�B
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


