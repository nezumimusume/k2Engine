

#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// �J�����N���X�B
	/// </summary>
	class Camera : public Noncopyable {
	public:
		/// <summary>
		/// �ˉe�s��̍X�V���@�B
		/// </summary>
		enum EnUpdateProjMatrixFunc {
			enUpdateProjMatrixFunc_Perspective,		//�����ˉe�s��B���ߖ@���������G����肽���Ȃ炱�����B
			enUpdateProjMatrixFunc_Ortho,			//���s���e�B�Q�c�I�ȕ\�����������Ȃ炱�����B
		};
		/// <summary>
		/// �r���[�s��A�v���W�F�N�V�����s����X�V����B
		/// </summary>
		void Update();
		/// <summary>
		/// �����_�����_�Ƃ��ăJ��������]������B
		/// </summary>
		/// <param name="qRot">��]������N�H�[�^�j�I��</param>
		void RotateOriginTarget(const Quaternion& qRot);

		/// <summary>
		/// �J�����𓮂����B
		/// </summary>
		/// <param name="move">��������</param>
		void Move(const Vector3& move)
		{
			m_position += move;
			m_target += move;
			m_isDirty = true;
		}
		/// <summary>
		/// �����_�𓮂����B
		/// </summary>
		/// <param name="move">�ړ���</param>
		void MoveTarget(const Vector3& move)
		{
			m_target += move;
			m_isDirty = true;
		}
		/// <summary>
		/// ���_�𓮂����B
		/// </summary>
		/// <param name="move"></param>
		void MovePosition(const Vector3& move)
		{
			m_position += move;
			m_isDirty = true;
		}
		/// <summary>
		/// �J�����̑O�������Ɉړ��B
		/// </summary>
		/// <param name="moveForward"></param>
		void MoveForward(float moveForward)
		{
			Move(m_forward * moveForward);
		}
		/// <summary>
		/// �J�����̉E�����Ɉړ��B
		/// </summary>
		/// <param name="moveRight"></param>
		void MoveRight(float moveRight)
		{
			Move(m_right * moveRight);
		}
		/// <summary>
		/// �J�����̏�����Ɉړ��B
		/// </summary>
		/// <param name="moveUp"></param>
		void MoveUp(float moveUp)
		{
			Move(m_up * moveUp);
		}
		/// <summary>
		/// �J�����̍��W��ݒ肷��B
		/// </summary>
		void SetPosition(const Vector3& pos)
		{
			m_position = pos;
			m_isDirty = true;
		}
		void SetPosition(float x, float y, float z)
		{
			SetPosition({ x, y, z });
		}
		/// <summary>
		/// �J�����̍��W���擾�B
		/// </summary>
		const Vector3& GetPosition() const
		{
			return m_position;
		}
		/// <summary>
		/// �����_��ݒ�B
		/// </summary>
		void SetTarget(float x, float y, float z)
		{
			SetTarget({ x, y, z });
		}
		void SetTarget(const Vector3& target)
		{
			m_target = target;
			m_isDirty = true;
		}
		/// <summary>
		/// �����_���擾�B
		/// </summary>
		const Vector3& GetTarget() const
		{
			return m_target;
		}
		/// <summary>
		/// �J�����̏������ݒ�B
		/// </summary>
		void SetUp(const Vector3& up)
		{
			m_up = up;
			m_up.Normalize();
		}
		void SetUp(float x, float y, float z)
		{
			SetUp({ x, y, z });
		}
		/// <summary>
		/// �J�����̏�������擾�B
		/// </summary>
		const Vector3& GetUp() const
		{
			return m_up;
		}

		/// <summary>
		/// �r���[�s��̋t�s����擾�B
		/// </summary>
		const Matrix& GetViewMatrixInv()
		{
			if (m_isDirty) {
				//�X�V����K�v������B
				Update();
			}
			return m_viewMatrixInv;
		}
		/// <summary>
		/// �r���[�s����擾�B
		/// </summary>
		const Matrix& GetViewMatrix()
		{
			if (m_isDirty) {
				//�X�V����K�v������B
				Update();
			}
			return m_viewMatrix;
		}
		/// <summary>
		/// �v���W�F�N�V�����s��̋t�s����擾�B
		/// </summary>
		const Matrix& GetProjectionMatrixInv()
		{
			if (m_isDirty) {
				//�X�V����K�v������B
				Update();
			}
			return m_projectionMatrixInv;
		}
		/// <summary>
		/// �v���W�F�N�V�����s����擾�B
		/// </summary>
		const Matrix& GetProjectionMatrix()
		{
			if (m_isDirty) {
				//�X�V����K�v������B
				Update();
			}
			return m_projectionMatrix;
		}
		/// <summary>
		/// �r���[�~�v���W�F�N�V�����s����擾�B
		/// </summary>
		const Matrix& GetViewProjectionMatrix()
		{
			if (m_isDirty) {
				//�X�V����K�v������B
				Update();
			}
			return m_viewProjectionMatrix;
		}
		/// <summary>
		/// �r���[�v���W�F�N�V�����s��̋t�s����v�Z�B
		/// </summary>
		/// <returns></returns>
		const Matrix& GetViewProjectionMatrixInv()
		{
			if (m_isDirty) {
				// �X�V����K�v������B
				Update();
			}
			return m_viewProjectionMatrixInv;
		}
		/// <summary>
		/// �J�����̉�]�s����擾�B
		/// </summary>
		const Matrix& GetCameraRotation()
		{
			if (m_isDirty) {
				//�X�V����K�v������B
				Update();
			}
			return m_cameraRotation;
		}
		/// <summary>
		/// �����ʂ܂ł̋�����ݒ�B
		/// </summary>
		void SetFar(float fFar)
		{
			m_far = fFar;
			m_isDirty = true;
		}
		/// <summary>
		/// �ߕ��ʂ܂ł̋�����ݒ�B
		/// </summary>
		void SetNear(float fNear)
		{
			m_near = fNear;
			m_isDirty = true;
		}
		/// <summary>
		/// �����ʂ܂ł̋������擾�B
		/// </summary>
		float GetFar() const
		{
			return m_far;
		}
		/// <summary>
		/// �ߕ��ʂ܂ł̋������擾�B
		/// </summary>
		float GetNear() const
		{
			return m_near;
		}
		/// <summary>
		/// ���s���e�̕���ݒ�B
		/// </summary>
		/// <remarks>
		/// SetUpdateProjMatrixFunc��enUpdateProjMatrixFunc_Ortho���ݒ肳��Ă���Ƃ��Ɏg�p�����B
		/// </remarks>
		void SetWidth(float w)
		{
			m_width = w;
			m_isDirty = true;
		}
		/// <summary>
		/// ���s���e�̍�����ݒ�B
		/// </summary>
		/// <remarks>
		/// SetUpdateProjMatrixFunc��enUpdateProjMatrixFunc_Ortho���ݒ肳��Ă���Ƃ��Ɏg�p�����B
		/// </remarks>
		void SetHeight(float h)
		{
			m_height = h;
			m_isDirty = true;
		}
		/// <summary>
		/// ���s���e�̕����擾�B
		/// </summary>
		float GetWidth() const
		{
			return m_width;
		}
		/// <summary>
		/// ���s���e�̍������擾�B
		/// </summary>
		float GetHeight() const
		{
			return m_height;
		}
		/// <summary>
		/// ���s���e�̍�����ݒ�B
		/// </summary>
		/// <remarks>
		/// SetUpdateProjMatrixFunc��enUpdateProjMatrixFunc_Ortho���ݒ肳��Ă���Ƃ��Ɏg�p�����B
		/// </remarks>
		void SetUpdateProjMatrixFunc(EnUpdateProjMatrixFunc func)
		{
			m_updateProjMatrixFunc = func;
			m_isDirty = true;
		}
		/// <summary>
		/// ��p��ݒ�B
		/// </summary>
		/// <remarks>
		/// SetUpdateProjMatrixFunc��enUpdateProjMatrixFunc_Perspective���ݒ肳��Ă���Ƃ��Ɏg�p�����B
		/// </remarks>
		/// <param name="viewAngle">��p�B�P�ʃ��W�A��</param>
		void SetViewAngle(float viewAngle)
		{
			m_viewAngle = viewAngle;
			m_isDirty = true;
		}
		/// <summary>
		/// ��p���擾�B
		/// </summary>
		/// <returns>��p�B�P�ʃ��W�A��</returns>
		float GetViewAngle() const
		{
			return m_viewAngle;
		}
		/// <summary>
		/// �����_�Ǝ��_�̋������擾�B
		/// </summary>
		/// <returns></returns>
		float GetTargetToPositionLength() const
		{
			return m_targetToPositionLen;
		}
		/// <summary>
		/// �J�����̑O�������擾�B
		/// </summary>
		const Vector3& GetForward() const
		{
			return m_forward;
		}
		/// <summary>
		/// �J�����̉E�������擾�B
		/// </summary>

		const Vector3& GetRight() const
		{
			return m_right;
		}
		/// <summary>
		/// �A�X�y�N�g����擾�B
		/// </summary>
		float GetAspect() const
		{
			return m_aspect;
		}
		/// <summary>
		/// �J�����̃R�s�[���쐬�B
		/// </summary>
		/// <param name="dst"></param>
		void CopyTo(Camera& dst)
		{
			memcpy(&dst, this, sizeof(dst));
			m_isDirty = true;
		}
		/// <summary>
		/// ���[���h���W����X�N���[�����W���v�Z����B
		/// </summary>
		/// <remarks>
		/// �v�Z�����X�N���[�����W�͉�ʂ̒��S��{0,0}�A�����{��ʂ̕�*-0.5,��ʂ̍���*-0.5}
		/// �E����{ ��ʂ̕� * 0.5,��ʂ̍��� * 0.5 }�Ƃ�����W�n�ł��B
		/// </remarks>
		/// <param name="screenPos">�X�N���[�����W�̊i�[��</param>
		/// <param name="worldPos">���[���h���W</param>
		void CalcScreenPositionFromWorldPosition(Vector2& screenPos, const Vector3& worldPos) const;

	protected:
		float		m_targetToPositionLen = 1.0f;			// �����_�Ǝ��_�܂ŋ����B
		Vector3		m_position = { 0.0f, 0.0f, 1.0f };		// �J�����ʒu�B
		Vector3		m_up = g_vec3Up;						// �J�����̏�����B
		Vector3		m_target;								// �J�����̒��~�_�B
		Matrix		m_viewMatrix;							// �r���[�s��B
		Matrix		m_projectionMatrix;						// �v���W�F�N�V�����s��B
		Matrix		m_viewProjectionMatrix;					// �r���[�v���W�F�N�V�����s��B
		Matrix		m_viewProjectionMatrixInv;				// �r���[�v���W�F�N�V�����s��̋t�s��
		Matrix		m_viewMatrixInv;						// �r���[�s��̋t�s��B
		Matrix      m_projectionMatrixInv;					// �v���W�F�N�V�����s��̋t�s��B
		Matrix		m_cameraRotation;						// �J�����̉�]�s��B
		Vector3		m_forward = g_vec3Front;				// �J�����̑O���B
		Vector3		m_right = g_vec3Right;					// �J�����̉E�B
		float		m_near = 1.0f;							// �ߕ��ʁB
		float		m_far = 5000.0f;						// �����ʁB
		float		m_viewAngle = Math::DegToRad(60.0f);	// ��p(���W�A��)�B
		float		m_aspect = 1.0f;						// �A�X�y�N�g��B
		float		m_width = 1280.0f;						// ���s���e�s����쐬����Ƃ��Ɏg�p����镝�B
		float		m_height = 720.0f;						// ���s���e�s����쐬����Ƃ��Ɏg�p����鍂���B
		EnUpdateProjMatrixFunc m_updateProjMatrixFunc = enUpdateProjMatrixFunc_Perspective;	// �v���W�F�N�V�����s��̍X�V�̎d���B
		bool		m_isNeedUpdateProjectionMatrix = true;
		bool		m_isDirty = false;						// �_�[�e�B�t���O�B
	};
}