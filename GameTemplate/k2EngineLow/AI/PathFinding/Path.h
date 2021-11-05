#pragma once

namespace nsK2EngineLow {
	namespace nsAI {
		class NaviMesh;
		/// <summary>
		/// �p�X�������N���X�B
		/// </summary>
		class Path{
		public:
			/// <summary>
			/// �p�X����ړ�����B
			/// </summary>
			/// <param name="pos">�ړ���������W</param>
			/// <param name="moveSpeed">�ړ����x</param>
			/// <param name="isEnd">�p�X�ړ��I��������true���ݒ肳���</param>
			/// <param name="physicsWorld">
			/// �������[���h�B
			/// �������[���h���w�肳��Ă���ƁA�p�X�ړ���ɒn�ʂɃ��C�L���X�g���s���A
			/// ���W��n�ʂɃX�i�b�v���܂��B
			/// </param>
			/// <returns>�ړ���̍��W</returns>
			Vector3 Move( 
				Vector3 pos, 
				float moveSpeed, 
				bool& isEnd,
				PhysicsWorld* physicsWorld = nullptr
			);
			/// <summary>
			/// �N���A
			/// </summary>
			void Clear()
			{
				m_sectionNo = 0;
				m_pointArray.clear();
				m_sectionArray.clear();
			}
			/// <summary>
			/// �|�C���g�̒ǉ��B
			/// </summary>
			/// <param name="point">�|�C���g</param>
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
			int m_sectionNo = 0;					// �Z�N�V�����ԍ��B
		};
	}
}

