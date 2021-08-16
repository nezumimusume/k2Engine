#pragma once
#include "physics/PhysicsGhostObject.h"


namespace nsK2Engine {
	/// <summary>
	/// ���̃R���W�����Ɠ����蔻����s���N���X�B
	/// </summary>
	class CollisionObject : public IGameObject
	{
	public:
		CollisionObject();
		~CollisionObject();
		bool Start();
		void Update();
		/// <summary>
	/// �{�b�N�X�`��̃S�[�X�g�I�u�W�F�N�g���쐬�B
	/// </summary>
	/// <param name="pos">���W�B</param>
	/// <param name="rot">��]�B</param>
	/// <param name="size">�T�C�Y�B</param>
		void CreateBox(Vector3 pos, Quaternion rot, Vector3 size)
		{
			m_physicsGhostObject.CreateBox(pos, rot, size);
		}
		/// <summary>
		/// �J�v�Z���`��̃S�[�X�g�I�u�W�F�N�g���쐬�B
		/// </summary>
		/// <param name="pos">���W�B</param>
		/// <param name="rot">��]�B</param>
		/// <param name="radius">�J�v�Z���̔��a�B</param>
		/// <param name="height">�J�v�Z���̍����B</param>
		void CreateCapsule(Vector3 pos, Quaternion rot, float radius, float height)
		{
			m_physicsGhostObject.CreateCapsule(pos, rot, radius, height);
		}
		/// <summary>
		/// ���`��̃S�[�X�g�I�u�W�F�N�g���쐬�B
		/// </summary>
		/// <param name="pos">���W�B</param>
		/// <param name="rot">��]�B</param>
		/// <param name="radius">���`�̔��a�B</param>
		void CreateSphere(Vector3 pos, Quaternion rot, float radius)
		{
			m_physicsGhostObject.CreateSphere(pos, rot, radius);
		}
		/// <summary>
		/// ���b�V���`��̃S�[�X�g�I�u�W�F�N�g���쐬�B
		/// </summary>
		/// <param name="pos">���W�B</param>
		/// <param name="rot">��]�B</param>
		/// <param name="model">���f���B</param>
		/// <param name="worldMatrix">���[���h�s��B</param>
		void CreateMesh(Vector3 pos, Quaternion rot, const Model& model, const Matrix& worldMatrix)
		{
			m_physicsGhostObject.CreateMesh(pos, rot, model, worldMatrix);
		}
		/// <summary>
		/// �ݒ肵������(�b)���o�߂���Ǝ����ō폜����܂��B0.0f��ݒ肵����1�t���[���o�߂ō폜����܂��B
		/// </summary>
		/// <param name="timeLimit">�폜����������(�b)�B</param>
		void SetTimeLimit(const float timeLimit)
		{
			m_timeLimit = timeLimit;
		}
		/// <summary>
		/// ���O��ݒ�B
		/// </summary>
		/// <param name="name">���O�B</param>
		void SetName(const char* name)
		{
			m_name = name;
		}
		/// <summary>
		/// ���O���擾�B
		/// </summary>
		/// <returns></returns>
		const char* GetName() const
		{
			return m_name;
		}
		/// <summary>
		/// ���W��ݒ�B
		/// </summary>
		/// <param name="position"></param>
		void SetPosition(const Vector3& position)
		{
			m_physicsGhostObject.SetPosition(position);
		}
		/// <summary>
		/// ��]��ݒ�B
		/// </summary>
		/// <param name="rotation"></param>
		void SetRotation(const Quaternion& rotation)
		{
			m_physicsGhostObject.SetRotation(rotation);
		}
		/// <summary>
		/// �s���ݒ�B
		/// </summary>
		/// <param name="rotation">�s��B</param>
		void SetWorldMatrix(const Matrix& matrix)
		{
			Vector3 position;
			position.x = matrix.m[3][0];
			position.y = matrix.m[3][1];
			position.z = matrix.m[3][2];
			SetPosition(position);
			Quaternion rotation;
			rotation.SetRotation(matrix);
			SetRotation(rotation);
		}
		/// <summary>
		/// �����ō폜���邩�ǂ�����ݒ肷��B
		/// </summary>
		/// <param name="isFlag">true�Ȃ玩���ō폜�����Bfalse�Ȃ玩���ō폜����Ȃ��̂ŁADeleteGO������K�v������B</param>
		void SetIsEnableAutoDelete(bool isFlag)
		{
			m_isEnableAutoDelete = isFlag;
		}
		/// <summary>
		/// �R���W�����I�u�W�F�N�g���m�̓����蔻��B
		/// </summary>
		/// <param name="collisionObject">�Փ˔��肵�����R���W�����I�u�W�F�N�g�B</param>
		/// <returns>�Փ˂�����true�B</returns>
		const bool IsHit(CollisionObject* collisionObject) const
		{
			bool isCollision = false;
			PhysicsWorld::GetInstance()->ContactTest(&collisionObject->GetbtCollisionObject(), [&](const btCollisionObject& contactObject) {
				if (m_physicsGhostObject.IsSelf(contactObject) == true) {
					isCollision = true;
				}
			});
			return isCollision;
		}
		/// <summary>
		/// �R���W�����I�u�W�F�N�g�ƃL�����R���̓����蔻��B
		/// </summary>
		/// <param name="collisionObject">�Փ˔��肵�����L�����R���B</param>
		/// <returns>�Փ˂�����true�B</returns>
		const bool IsHit(CharacterController& characterController) const
		{
			bool isCollision = false;
			PhysicsWorld::GetInstance()->ContactTest(characterController, [&](const btCollisionObject& contactObject) {
				if (m_physicsGhostObject.IsSelf(contactObject) == true) {
					isCollision = true;
				}
			});
			return isCollision;
		}
		//���̂��������Ȃ��̂Ō��݂͎g�p�s�B
		/*/// <summary>
		/// �R���W�����I�u�W�F�N�g�ƃt�B�W�b�N�X�X�^�e�B�b�N�I�u�W�F�N�g�̓����蔻��B
		/// </summary>
		/// <param name="physicsStaticObject">�Փ˔��肵�����t�B�W�b�N�X�X�^�e�B�b�N�I�u�W�F�N�g�B</param>
		/// <returns>�Փ˂�����true�B</returns>
		bool IsHit(PhysicsStaticObject& physicsStaticObject)
		{
			bool isCollision = false;
			PhysicsWorld::GetInstance()->ContactTest(physicsStaticObject.GetbtCollisionObject(), [&](const btCollisionObject& contactObject) {
				if (m_physicsGhostObject.IsSelf(contactObject) == true) {
					isCollision = true;
				}
				});
			return isCollision;
		}*/
		/// <summary>
		/// �S�[�X�g�I�u�W�F�N�g���擾�B
		/// </summary>
		/// <returns>�S�[�X�g�I�u�W�F�N�g�B</returns>
		PhysicsGhostObject& GetGhostObject()
		{
			return m_physicsGhostObject;
		}
		/// <summary>
		/// bt�R���W�����I�u�W�F�N�g���擾�B
		/// </summary>
		/// <returns>bt�R���W�����I�u�W�F�N�g�B</returns>
		btCollisionObject& GetbtCollisionObject()
		{
			return m_physicsGhostObject.GetbtCollisionObject();
		}
		/// <summary>
		/// �����蔻�肪�L�����ǂ�����ݒ肷��B
		/// </summary>
		/// <param name="isEnable">�L���ɂ������Ȃ�true�B</param>
		void SetIsEnable(bool isEnable)
		{
			m_isEnable = isEnable;
		}
		/// <summary>
		/// �����蔻�肪�L�����ǂ������擾����B
		/// </summary>
		/// <returns>�����蔻�肪�L���Ȃ�true�B</returns>
		const bool IsEnable() const
		{
			return m_isEnable;
		}
	private:
		PhysicsGhostObject			m_physicsGhostObject;				//�S�[�X�g�I�u�W�F�N�g�B
		const char* m_name = nullptr;					//���O�B
		float						m_timer = 0.0f;						//�^�C�}�[�B
		float						m_timeLimit = 0.0f;					//�폜�����܂ł̎��ԁB
		bool						m_isEnableAutoDelete = true;		//�����ō폜�����Ȃ�true�B
		bool						m_isEnable = true;					//true�Ȃ瓖���蔻��L���B
	};

	class CollisionObjectManager
	{
	public:
		CollisionObjectManager();
		~CollisionObjectManager();
		void AddCollisionObject(CollisionObject* collisionObject)
		{
			m_collisionObjectVector.push_back(collisionObject);
		}
		/// <summary>
		/// ���O�����S��v����R���W�����I�u�W�F�N�g����������B�������1�����B
		/// </summary>
		/// <param name="name">���O�B</param>
		/// <returns>�R���W�����I�u�W�F�N�g�B������Ȃ������ꍇ��nullptr�B</returns>
		CollisionObject* FindCollisionObject(const char* name)
		{
			for (auto collisionObject : m_collisionObjectVector)
			{
				//���O��v�I
				if (strcmp(collisionObject->GetName(), name) == 0)
				{
					//�����蔻�肪�L���Ȃ�B
					if (collisionObject->IsEnable() == true)
					{
						return collisionObject;
					}
				}
			}
			return nullptr;
		}
		/// <summary>
		/// ���O���O����v����R���W�����I�u�W�F�N�g����������B�������1�����B
		/// </summary>
		/// <param name="name">���O�B</param>
		/// <returns>�R���W�����I�u�W�F�N�g�B������Ȃ������ꍇ��nullptr�B</returns>
		const CollisionObject* FindMatchForwardNameCollisionObject(const char* name) const
		{
			for (auto collisionObject : m_collisionObjectVector)
			{
				auto len = strlen(name);
				auto namelen = strlen(collisionObject->GetName());
				if (len > namelen) {
					//���O�������B�s��v�B
					continue;
				}
				if (strncmp(name, collisionObject->GetName(), len) == 0)
				{
					//�����蔻�肪�L���Ȃ�B
					if (collisionObject->IsEnable() == true)
					{
						return collisionObject;
					}
				}
			}
			return nullptr;
		}
		/// <summary>
		/// ���O�����S��v����R���W�����I�u�W�F�N�g����������B������͕����B
		/// </summary>
		/// <param name="name">���O�B</param>
		/// <returns>�R���W�����I�u�W�F�N�g�̔z��B</returns>
		const std::vector<CollisionObject*>& FindCollisionObjects(const char* name)
		{
			m_findMatchForwardNameCollisionObjectVector.clear();
			for (auto collisionObject : m_collisionObjectVector)
			{
				//���O��v�I
				if (strcmp(collisionObject->GetName(), name) == 0)
				{
					//�����蔻�肪�L���Ȃ�B
					if (collisionObject->IsEnable() == true)
					{
						m_findMatchForwardNameCollisionObjectVector.push_back(collisionObject);
					}
				}
			}
			return m_findMatchForwardNameCollisionObjectVector;
		}
		/// <summary>
		/// ���O���O����v����R���W�����I�u�W�F�N�g����������B������͕����B
		/// </summary>
		/// <param name="name">���O�B</param>
		/// <returns>�R���W�����I�u�W�F�N�g�̔z��B</returns>
		const std::vector<CollisionObject*>& FindMatchForwardNameCollisionObjects(const char* name)
		{
			m_findsCollisionObjectVector.clear();
			for (auto collisionObject : m_collisionObjectVector)
			{
				auto len = strlen(name);
				auto namelen = strlen(collisionObject->GetName());
				if (len > namelen) {
					//���O�������B�s��v�B
					continue;
				}
				if (strncmp(name, collisionObject->GetName(), len) == 0)
				{
					//�����蔻�肪�L���Ȃ�B
					if (collisionObject->IsEnable() == true)
					{
						m_findsCollisionObjectVector.push_back(collisionObject);
					}
				}
			}
			return m_findsCollisionObjectVector;
		}
		/// <summary>
		/// �z�񂩂�R���W�����I�u�W�F�N�g���폜�B
		/// </summary>
		/// <param name="deleteCollisionObject">�폜�������R���W�����I�u�W�F�N�g�B</param>
		void RemoveCollisionObject(CollisionObject* deleteCollisionObject)
		{
			for (auto it = m_collisionObjectVector.begin(); it != m_collisionObjectVector.end();) {
				// ������v�����v�f���폜����
				if (*it == deleteCollisionObject) {
					// �폜���ꂽ�v�f�̎����w���C�e���[�^���Ԃ����B
					it = m_collisionObjectVector.erase(it);
				}
				// �v�f�폜�����Ȃ��ꍇ�ɁA�C�e���[�^��i�߂�
				else {
					++it;
				}
			}
		}
	private:
		std::vector<CollisionObject*>		m_collisionObjectVector;
		std::vector<CollisionObject*>		m_findsCollisionObjectVector;
		std::vector<CollisionObject*>		m_findMatchForwardNameCollisionObjectVector;
	};
	
}