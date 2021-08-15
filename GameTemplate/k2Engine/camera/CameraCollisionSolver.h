#pragma once

namespace nsK2Engine {
	/// <summary>
	/// �J�����̃R���W�����\���o�[�B
	///
	/// ��ʓI�ȎO�l�̎��_�̃Q�[���J�����̏Փˉ�����񋟂��܂��B
	/// �J������w�i�ɂ߂荞�܂������Ȃ��ꍇ�ȂǂɎg�p���Ă��������B
	///
	///�T�^�I�Ȏg�p��B
	///int main()
	///{
	///	�E
	///		�E
	///		�ȗ�
	///		�Q�[���̏������Ȃǂ��s���B�B
	///		�E
	///		�E
	///		CameraCollisionSolver cameraCollisionSolver;
	///		//�J�����̃R���W�����\���o�[������������B
	///		cameraCollisionSolver.Init(50.0f);
	///		while (true) {	//�Q�[�����[�v
	///		�E
	///			�E
	///			�ȗ�
	///			���̃t���[���̃J�����̎��_�⒍�ӓ_���v�Z�����肷��B
	///			�E
	///			�E
	///			Vector3 newCameraPosition;	//�V�����J�����̎��_�̊i�[��B
	///			cameraCollisionSolver.Execute(
	///			newCameraPosition,		//�������͏Փˉ�����̎��_�̍��W�̊i�[��B
	///			cameraPosition,			//�������͏Փˉ����O�̎��_�̍��W�B
	///			cameraTarget,			//��O�����͒����_�̍��W�B
	///			//�Փˉ����������W���J�����̍��W�Ƃ��Đݒ肷��B
	///			g_camera3D->SetPosition(newCameraPosition);
	///		}
	///
	///		return 0;
	///}
	/// </summary>
	class CameraCollisionSolver : public Noncopyable
	{
	public:
		/// <summary>
		/// �R���X�g���N�^�B
		/// </summary>
		CameraCollisionSolver();
		/// <summary>
		/// �f�X�g���N�^�B
		/// </summary>
		~CameraCollisionSolver();
		/// <summary>
		/// �������B
		/// </summary>
		/// <param name="radius">�J�����̃o�E���f�B���O�X�t�B�A�̔��a�B</param>
		void Init(float radius);
		/// <summary>
		/// �R���W���������̎��s�B
		/// </summary>
		/// <param name="result">�R���W�����������s�����_�̍��W���i�[�����B</param>
		/// <param name="position">�R���W�����������s���O�̎��_�̍��W�B</param>
		/// <param name="target">�J�����̒����_�B</param>
		/// <returns>�R���W�����������s�����ꍇ��true���Ԃ��Ă���B</returns>
		bool Execute(Vector3& result, const Vector3& position, const Vector3& target);
	private:
		SphereCollider		m_collider;			//�R���C�_�[�B
		float				m_radius = 0.0f;	//���a�B
		bool				m_isInited = false;	//�������ς݂̃t���O�Btrue�ɂȂ��Ă��珉�����ς݁B
	};
}
