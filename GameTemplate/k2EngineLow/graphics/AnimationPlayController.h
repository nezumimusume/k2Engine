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
		/*!
		* @brief	�A�j���[�V�����N���b�v�̕ύX�B
		*/
		void ChangeAnimationClip(AnimationClip* clip);

		void SetInterpolateTime(float interpolateTime)
		{
			if (interpolateTime < 0.0f) {
				//interpolateTime is negative!!
				return;
			}
			m_interpolateEndTime = interpolateTime;
			m_interpolateTime = 0.0f;
		}
		/*!
			*@brief	�⊮�����擾�B
			*/
		float GetInterpolateRate() const
		{
			if (m_interpolateEndTime <= 0.0f) {
				return 1.0f;
			}
			return min(1.0f, m_interpolateTime / m_interpolateEndTime);
		}
		/*!
		* @brief	�A�j���[�V������i�߂�B
		*@param[in]	deltaTime		�A�j���[�V������i�߂鎞�ԁB
		*/
		void Update(float deltaTime, Animation* animation, bool isInvokeAnimationEvent);
		/*!
			* @brief	���[�J���{�[���s����擾�B
			*/
		const std::vector<Matrix>& GetBoneLocalMatrix() const
		{
			return m_boneMatrix;
		}
		AnimationClip* GetAnimClip() const
		{
			return m_animationClip;
		}
		/*!
		* @brief	�Đ����H
		*/
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
		/*!
		*@brief	�A�j���[�V�����C�x���g���N������B
		*/
		void InvokeAnimationEvent(Animation* animation);
		/*!
		*@brief	���[�v�Đ��J�n���鎞�̏����B
		*/
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
		AnimationClip* m_animationClip = nullptr;			//�A�j���[�V�����N���b�v�B
		int						m_currentKeyFrameNoLastFrame = 0;	//��t���[���O�̃L�[�t���[���ԍ��B
		int						m_currentKeyFrameNo = 0;			//���ݍĐ����̃L�[�t���[���ԍ��B
		float					m_time = 0.0f;
		std::vector<Matrix>		m_boneMatrix;						//!<���̃R���g���[���ōĐ����̃A�j���[�V�����̃{�[���s��B
		float					m_interpolateTime;					//!<�⊮����
		float					m_interpolateEndTime;				//!<�⊮�I������
		bool					m_isPlaying = false;				//!<�Đ����H
		Skeleton* m_skeleton = nullptr;				//!<�X�P���g���B
		Vector3					m_footstepDeltaValue = g_vec3Zero;	//�t�b�g�X�e�b�v�̈ړ��x�N�g���B
		Vector3					m_footstepPos = g_vec3Zero;			//�t�b�g�X�e�b�v�{�[���̍��W�B
		int						m_footstepBoneNo = -1;				//�t�b�g�X�e�b�v�̃{�[���̔ԍ��B
	};
}