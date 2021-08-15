/*!
* @brief	�A�j���[�V�����Đ��R���g���[���B
*/

#include "k2EngineLowPreCompile.h"
#include "AnimationPlayController.h"
#include "AnimationClip.h"
#include "Animation.h"
#include "Skeleton.h"

namespace nsK2EngineLow {
	void AnimationPlayController::Init(Skeleton* skeleton, int footStepBoneNo)
	{
		m_footstepBoneNo = footStepBoneNo;
		int numBones = skeleton->GetNumBones();
		//�{�[���s����o�V�b�Ɗm�ہB
		m_boneMatrix.resize(numBones);
		m_skeleton = skeleton;
	}
	void AnimationPlayController::ChangeAnimationClip(AnimationClip* clip)
	{
		m_animationClip = clip;
		m_currentKeyFrameNo = 0;
		m_time = 0.0f;
		m_isPlaying = true;
		m_footstepPos = g_vec3Zero;
		m_footstepDeltaValue = g_vec3Zero;
		//�A�j���[�V�����C�x���g�̔����t���O�����ׂ�false�ɂ���B
		auto& animEventArray = m_animationClip->GetAnimationEvent();
		for (auto i = 0; i < m_animationClip->GetNumAnimationEvent(); i++) {
			animEventArray[i].SetInvokedFlag(false);
		}
	}
	void AnimationPlayController::InvokeAnimationEvent(Animation* animation)
	{
		auto& animEventArray = m_animationClip->GetAnimationEvent();
		for (auto i = 0; i < m_animationClip->GetNumAnimationEvent(); i++) {
			if (m_time > animEventArray[i].GetInvokeTime()
				&& animEventArray[i].IsInvoked() == false) {
				//�A�j���[�V�����̋N�����Ԃ��߂��Ă��銎�A�܂��C�x���g�N�����Ă��Ȃ��B
				animation->NotifyAnimationEventToListener(
					m_animationClip->GetName(), animEventArray[i].GetEventName()
				);
				animEventArray[i].SetInvokedFlag(true);
			}
		}
	}

	void AnimationPlayController::StartLoop()
	{
		m_footstepPos = g_vec3Zero;
		m_currentKeyFrameNo = 0;
		m_time = 0.0f;
		//�A�j���[�V�����C�x���g�����ׂĖ������ɂ���B
		auto& animEventArray = m_animationClip->GetAnimationEvent();
		for (auto i = 0; i < m_animationClip->GetNumAnimationEvent(); i++) {
			animEventArray[i].SetInvokedFlag(false);
		}
	}
	void AnimationPlayController::CalcBoneMatrixInRootBoneSpace(Bone& bone, Matrix parentMatrix)
	{
		//���[���h�s����v�Z����B
		auto& mBoneInRootSpace = m_boneMatrix[bone.GetNo()];
		Matrix localMatrix = m_boneMatrix[bone.GetNo()];
		//�e�̍s��ƃ��[�J���s�����Z���āA���[���h�s����v�Z����B
		mBoneInRootSpace = localMatrix * parentMatrix;

		//�q���̃��[���h�s����v�Z����B
		for (auto& childBone : bone.GetChildren()) {
			CalcBoneMatrixInRootBoneSpace(*childBone, mBoneInRootSpace);
		}
	}
	void AnimationPlayController::SamplingBoneMatrixFromAnimationClip()
	{
		const auto& keyFramePtrListArray = m_animationClip->GetKeyFramePtrListArray();
		for (const auto& keyFrameList : keyFramePtrListArray) {
			if (keyFrameList.size() == 0) {
				continue;
			}
			//���ݍĐ����̃L�[�t���[��������Ă���B
			KeyFrame* keyframe = keyFrameList.at(m_currentKeyFrameNo);
			m_boneMatrix[keyframe->boneIndex] = keyframe->transform;
		}
	}
	void AnimationPlayController::CalcBoneMatrixInRootBoneSpace()
	{
		int numBone = m_skeleton->GetNumBones();
		for (int boneNo = 0; boneNo < numBone; boneNo++) {
			//���[�g�̍�����������B
			auto bone = m_skeleton->GetBone(boneNo);
			if (bone->GetParentBoneNo() != -1) {
				continue;
			}
			CalcBoneMatrixInRootBoneSpace(*bone, g_matIdentity);
		}
	}
	void AnimationPlayController::SamplingDeltaValueFootstepBone()
	{
		if (m_currentKeyFrameNoLastFrame == m_currentKeyFrameNo) {
			//�L�[�t���[�����i��ł��Ȃ��B
			return;
		}
		if (m_footstepBoneNo == -1) {
			return;
		}
		int numBone = m_skeleton->GetNumBones();

		for (int boneNo = 0; boneNo < numBone; boneNo++) {
			auto bone = m_skeleton->GetBone(boneNo);
			if (m_footstepBoneNo == bone->GetNo()) {
				auto mat = m_boneMatrix[bone->GetNo()];
				Vector3 footstepBonePos;
				footstepBonePos.x = mat.m[3][0];
				footstepBonePos.y = mat.m[3][1];
				footstepBonePos.z = mat.m[3][2];
				//���̃t���[���ł�footstep�̈ړ��ʂ��v�Z����B
				m_footstepDeltaValue = footstepBonePos - m_footstepPos;
				//���̃t���[���ł�footstep�̍��W���X�V����B
				m_footstepPos = footstepBonePos;
				break;
			}
		}
	}
	void AnimationPlayController::SubtractFootstepbonePosFromAllBone()
	{
		if (m_footstepBoneNo == -1) {
			return;
		}
		int numBone = m_skeleton->GetNumBones();

		for (int boneNo = 0; boneNo < numBone; boneNo++) {
			auto bone = m_skeleton->GetBone(boneNo);
			m_boneMatrix[bone->GetNo()].m[3][0] -= m_footstepPos.x;
			m_boneMatrix[bone->GetNo()].m[3][1] -= m_footstepPos.y;
			m_boneMatrix[bone->GetNo()].m[3][2] -= m_footstepPos.z;
		}
	}
	void AnimationPlayController::ProgressKeyframeNo(float deltaTime)
	{
		//1�t���[���O�̃L�[�t���[���ԍ����L�^���Ă����B
		m_currentKeyFrameNoLastFrame = m_currentKeyFrameNo;

		const auto& topBoneKeyFrameList = m_animationClip->GetTopBoneKeyFrameList();

		//�⊮���Ԃ��i�߂Ă����B
		m_interpolateTime = min(1.0f, m_interpolateTime + deltaTime);
		while (true) {
			if (m_currentKeyFrameNo >= (int)topBoneKeyFrameList.size()) {
				//�I�[�܂ōs�����B
				if (m_animationClip->IsLoop()) {
					//���[�v�B
					StartLoop();
				}
				else {
					//�����V���b�g�Đ��B
					m_currentKeyFrameNo--;
					m_isPlaying = false;	//�Đ��I���B
				}
				break;
			}
			if (topBoneKeyFrameList.at(m_currentKeyFrameNo)->time >= m_time) {
				//�I���B
				break;
			}
			//���ցB
			m_currentKeyFrameNo++;
		}
	}
	void AnimationPlayController::Update(
		float deltaTime,
		Animation* animation,
		bool isInvokeAnimationEvent)
	{
		if (m_animationClip == nullptr) {
			return;
		}

		m_time += deltaTime;

		if (isInvokeAnimationEvent) {
			//�A�j���[�V�����C�x���g�̔����B
			InvokeAnimationEvent(animation);
		}
		//�L�[�t���[���ԍ���i�߂�B
		ProgressKeyframeNo(deltaTime);

		//�{�[���s����A�j���[�V�����N���b�v����T���v�����O���Ă����B
		SamplingBoneMatrixFromAnimationClip();

		//�e�̍����W�n�ɂȂ��Ă���{�[���s������[�g�̃{�[���̋�Ԃɕϊ����Ă����B
		CalcBoneMatrixInRootBoneSpace();

		//footstep�{�[���̈ړ��ʂ��擾����B
		SamplingDeltaValueFootstepBone();

		//footstep�{�[���̈ړ��ʂ�S�̂̍����猸�Z����B
		SubtractFootstepbonePosFromAllBone();

		//�A�j���[�V�����Đ���������X�P���g���ɂ���B
		m_skeleton->SetMarkPlayAnimation();
	}
}
