#pragma once

#include "tkFile/TkaFile.h"

namespace nsK2EngineLow {
	/// <summary>
	/// �L�[�t���[��
	/// </summary>
	struct KeyFrame {
		uint32_t boneIndex;	//!<�{�[���C���f�b�N�X�B
		float time;					//!<���ԁB
		Matrix transform;			//!<�g�����X�t�H�[���B
	};

	/// <summary>
	/// �A�j���[�V�����C�x���g�B
	/// </summary>
	class AnimationEvent : public Noncopyable {
	public:


		/// <summary>
		/// �C�x���g�������Ԃ�ݒ�B
		/// </summary>
		/// <returns></returns>
		float GetInvokeTime() const
		{
			return m_invokeTime;
		}

		/// <summary>
		/// �C�x���g�����擾�B
		/// </summary>
		/// <returns></returns>
		const wchar_t* GetEventName() const
		{
			return m_eventName.c_str();
		}

		/// <summary>
		/// �C�x���g�������Ԃ�ݒ�B
		/// </summary>
		/// <param name="time"></param>
		void SetInvokeTime(float time)
		{
			m_invokeTime = time;
		}
		/*!
		*@brief	�C�x���g����ݒ�B
		*/

		/// <summary>
		/// �C�x���g����ݒ�
		/// </summary>
		/// <param name="name"></param>
		void SetEventName(const wchar_t* name)
		{
			m_eventName = name;
		}

		/// <summary>
		/// �C�x���g�������ς݂�����
		/// </summary>
		/// <returns></returns>
		bool IsInvoked() const
		{
			return m_isInvoked;
		}

		/// <summary>
		/// �C�x���g�����ς݂̃t���O��ݒ肷��B
		/// </summary>
		/// <param name="flag"></param>
		void SetInvokedFlag(bool flag)
		{
			m_isInvoked = flag;
		}
	private:
		bool m_isInvoked = false;	//!<�C�x���g�����ς݁H
		float m_invokeTime = 0.0f;	//!<�C�x���g�������ԁB
		std::wstring m_eventName;	//!<�C�x���g���B
	};

	/// <summary>
	/// �A�j���[�V�����N���b�v�N���X�B
	/// </summary>
	class AnimationClip {
	public:

		using keyFramePtrList = std::vector<KeyFrame*>;
		/// <summary>
		/// �R���X�g���N�^
		/// </summary>
		AnimationClip()
		{
		}
		
		/// <summary>
		/// �f�X�g���N�^�B
		/// </summary>
		~AnimationClip();
		/// <summary>
		/// �A�j���[�V�����N���b�v�𓯊����[�h�B
		/// </summary>
		/// <param name="filePath"></param>
		void Load(const char* filePath);

		/// <summary>
		/// �L�[�t���[���ƃA�j���[�V�����C�x���g���\�z����B
		/// </summary>
		/// <remarks>
		/// ���[�h���I������Ƃ��ɌĂяo���Ă��������B
		/// </remarks>
		void BuildKeyFramesAndAnimationEvents();
		/*!
		*@brief	���[�v����H
		*/
		bool IsLoop() const
		{
			return m_isLoop;
		}
		/*!
		*@brief	���[�v�t���O��ݒ肷��B
		*/
		void SetLoopFlag(bool flag)
		{
			m_isLoop = flag;
		}
		/*!
		*@brief
		*/
		const std::vector<keyFramePtrList>& GetKeyFramePtrListArray() const
		{
			return m_keyFramePtrListArray;
		}
		const keyFramePtrList& GetTopBoneKeyFrameList() const
		{
			return *m_topBoneKeyFramList;
		}
		
		/// <summary>
		/// �N���b�v�����擾�B
		/// </summary>
		/// <returns></returns>
		const wchar_t* GetName() const
		{
			return m_clipName.c_str();
		}
		
		/// <summary>
		/// �A�j���[�V�����C�x���g���擾�B
		/// </summary>
		/// <returns></returns>
		std::unique_ptr<AnimationEvent[]>& GetAnimationEvent()
		{
			return m_animationEvent;
		}

		/// <summary>
		/// �A�j���[�V�����C�x���g�̐����擾
		/// </summary>
		/// <returns></returns>
		int GetNumAnimationEvent() const
		{
			return m_tkaFile->GetNumAnimationEvent();
		}
	private:
		using KeyframePtr = std::unique_ptr<KeyFrame>;
		std::wstring m_clipName;										//!<�A�j���[�V�����N���b�v�̖��O�B
		bool m_isLoop = false;											//!<���[�v�t���O�B
		std::vector<KeyframePtr>			m_keyframes;				//�L�[�t���[���B
		std::vector<keyFramePtrList>		m_keyFramePtrListArray;		//�{�[�����Ƃ̃L�[�t���[���̃��X�g���Ǘ����邽�߂̔z��B
		std::unique_ptr<AnimationEvent[]>	m_animationEvent;			//�A�j���[�V�����C�x���g�B
		int									m_numAnimationEvent = 0;	//�A�j���[�V�����C�x���g�̐��B
		keyFramePtrList* m_topBoneKeyFramList = nullptr;
		TkaFile*							m_tkaFile = nullptr;		//tka�t�@�C��
	};
	using AnimationClipPtr = std::unique_ptr<AnimationClip>;
}