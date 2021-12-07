/*!
* @brief	�A�j���[�V�����Đ��R���g���[���B
*/

#pragma once


namespace nsK2EngineLow {
	class Skeleton;
	class AnimationClip;
	class Animation;
	class Bone;

	/*!
	* @brief	�A�j���[�V�����̍Đ��R���g���[���B
	* @details
	*  ��̃A�j���[�V�����N���b�v�ɑ΂��ăA�j���[�V������i�߂āA���[�J���|�[�Y���v�Z���܂��B
	*/

	/// <summary>
	/// �A�j���[�V�����̍Đ��R���g���[���B
	/// </summary>
	/// <remark>
	/// ��̃A�j���[�V�����N���b�v�ɑ΂��āA�A�j���[�V������i�߂ă��[�J���|�[�Y���v�Z���܂��B
	/// </remark>
	class AnimationPlayController : public Noncopyable {
	public:
		/*!
		* @brief	�R���X�g���N�^�B
		*/
		AnimationPlayController()
		{
		}
		/*!
		* @brief	�f�X�g���N�^�B
		*/
		~AnimationPlayController()
		{
		}
		/// <summary>
		/// ������
		/// </summary>
		/// <param name="skeleton">�X�P���g��</param>
		/// <param name="footStepBoneNo">footstep�{�[���̔ԍ�</param>
		void Init(Skeleton* skeleton, int footStepBoneNo);

		/// <summary>
		/// �A�j���[�V�����N���b�v�̕ύX
		/// </summary>
		/// <param name="clip">�ύX��̃A�j���[�V�����N���b�v</param>
		void ChangeAnimationClip(AnimationClip* clip);

		/// <summary>
		/// �A�j���[�V������Ԏ��Ԃ�ݒ�B
		/// </summary>
		/// <param name="interpolateTime">��Ԏ���</param>
		void SetInterpolateTime(float interpolateTime)
		{
			if (interpolateTime < 0.0f) {
				//interpolateTime is negative!!
				return;
			}
			m_interpolateEndTime = interpolateTime;
			m_interpolateTime = 0.0f;
		}
		/// <summary>
		/// ���݂̃A�j���[�V������ԗ����擾
		/// </summary>
		/// <returns></returns>
		float GetInterpolateRate() const
		{
			if (m_interpolateEndTime <= 0.0f) {
				return 1.0f;
			}
			return min(1.0f, m_interpolateTime / m_interpolateEndTime);
		}
		
		/// <summary>
		/// �A�j���[�V������i�߂�
		/// </summary>
		/// <param name="deltaTime">�A�j���[�V������i�߂鎞��(�P��:�b)</param>
		/// <param name="animation"></param>
		/// <param name="isInvokeAnimationEvent"></param>
		void Update(float deltaTime, Animation* animation, bool isInvokeAnimationEvent);
		

		/// <summary>
		/// ���[�J���{�[���s����擾�B
		/// </summary>
		/// <returns></returns>
		const std::vector<Matrix>& GetBoneLocalMatrix() const
		{
			return m_boneMatrix;
		}

		/// <summary>
		/// �A�j���[�V�����N���b�v�̎擾�B
		/// </summary>
		/// <returns></returns>
		AnimationClip* GetAnimClip() const
		{
			return m_animationClip;
		}
		
		/// <summary>
		/// �A�j���[�V�������Đ���������B
		/// </summary>
		/// <returns></returns>
		bool IsPlaying() const
		{
			return m_isPlaying;
		}
		/// <summary>
		/// Update�֐����Ăяo�����Ƃ��́Afootstep�{�[���̈ړ��ʂ��擾�B
		/// </summary>
		/// <returns></returns>
		Vector3 GetFootStepDeltaValueOnUpdate() const
		{
			return m_footstepDeltaValue;
		}
	private:
		

		/// <summary>
		/// �A�j���[�V�����C�x���g���N������B
		/// </summary>
		/// <param name="animation"></param>
		void InvokeAnimationEvent(Animation* animation);
		
		/// <summary>
		/// ���[�v�Đ����J�n����Ƃ��Ɏ��s����鏈���B
		/// </summary>
		void StartLoop();
		/// <summary>
		/// ���[�g�̃{�[����Ԃł̃{�[���s����v�Z����B
		/// </summary>
		/// <param name="bone">�v�Z���鍜</param>
		/// <param name="parentMatrix">�e�̍s��</param>
		void CalcBoneMatrixInRootBoneSpace(Bone& bone, Matrix parentMatrix);
		/// <summary>
		/// �{�[���s����A�j���[�V�����N���b�v����T���v�����O����B
		/// </summary>
		void SamplingBoneMatrixFromAnimationClip();
		/// <summary>
		/// ���[�g�{�[����Ԃł̍s����v�Z����B
		/// </summary>
		void CalcBoneMatrixInRootBoneSpace();
		/// <summary>
		/// footstep�{�[���̕ω��ʂ��T���v�����O����B
		/// </summary>
		void SamplingDeltaValueFootstepBone();
		/// <summary>
		/// footstep�{�[���̍��W��S�̂̍����猸�Z����B
		/// </summary>
		void SubtractFootstepbonePosFromAllBone();
		/// <summary>
		/// �L�[�t���[���ԍ���i�߂�B
		/// </summary>
		void ProgressKeyframeNo(float deltaTime);
	private:
		AnimationClip* m_animationClip = nullptr;					// �A	�j���[�V�����N���b�v�B
		int						m_currentKeyFrameNoLastFrame = 0;	// ��t���[���O�̃L�[�t���[���ԍ��B
		int						m_currentKeyFrameNo = 0;			// ���ݍĐ����̃L�[�t���[���ԍ��B
		float					m_time = 0.0f;
		std::vector<Matrix>		m_boneMatrix;						// ���̃R���g���[���ōĐ����̃A�j���[�V�����̃{�[���s��B
		float					m_interpolateTime;					// �⊮����
		float					m_interpolateEndTime;				// �⊮�I������
		bool					m_isPlaying = false;				// �Đ����H
		Skeleton* m_skeleton = nullptr;								// �X�P���g���B
		Vector3					m_footstepDeltaValue = g_vec3Zero;	// �t�b�g�X�e�b�v�̈ړ��x�N�g���B
		Vector3					m_footstepPos = g_vec3Zero;			// �t�b�g�X�e�b�v�{�[���̍��W�B
		int						m_footstepBoneNo = -1;				// �t�b�g�X�e�b�v�̃{�[���̔ԍ��B
	};
}