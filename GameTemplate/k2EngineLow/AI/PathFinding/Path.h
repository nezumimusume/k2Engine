#pragma once

namespace nsK2EngineLow {
	namespace nsAI {
		
		class Path{
		public:
			/// <summary>
			/// �p�X�̈ړ��J�n�B
			/// </summary>
			void StartWalkPath();
			/// <summary>
			/// �p�X�ړ��B
			/// </summary>
			bool WalkPath( float moveSpeed );
			/// <summary>
			/// �|�C���g�̒ǉ��B
			/// </summary>
			/// <param name="point"></param>
			void AddPoint(const Vector3& point)
			{
				m_pointArray.push_back(point);
			}
			/// <summary>
			/// �p�X���\�z�B
			/// </summary>
			void Build();
		private:
			/// <summary>
			/// �p�X�̃Z�N�V����
			/// </summary>
			struct SSection {
				Vector3 startPos;	// �Z�N�V�����̊J�n���W�B
				Vector3 endPos;		// �Z�N�V�����̏I�����W�B
				Vector3 direction;	// �Z�N�V�����̕����B
				float length;		// �Z�N�V�����̒����B
			};
			std::vector<Vector3>	m_pointArray;	// �|�C���g�̔z��
			std::vector< SSection >	m_sectionArray;	// �Z�N�V�����̔z��B
		};
	}
}

