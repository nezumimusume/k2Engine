/// <summary>
/// tka�t�@�C��
/// </summary>
/// <remarks>
/// tka�t�@�C���̓L�[�t���[���A�j���[�V�����f�[�^�̃t�H�[�}�b�g�ł��B
/// �{�N���X�𗘗p����Ƃ��ł�tka�t�@�C�����������Ƃ��ł��܂��B
/// </remarks>
#pragma once

namespace nsK2EngineLow {

	class TkaFile : public Noncopyable {
	public:
		/// <summary>
		/// �A�j���[�V�����N���b�v�̃w�b�_�[�B
		/// </summary>
		struct AnimClipHeader {
			std::uint32_t numKey;				//�L�[�t���[���̐��B
			std::uint32_t numAnimationEvent;	//�A�j���[�V�����C�x���g�̐��B
		};
		/// <summary>
		/// �A�j���[�V�����C�x���g�B
		/// </summary>
		struct AnimationEvent {
			float	invokeTime;					//�A�j���[�V�����C�x���g���������鎞��(�P��:�b)
			std::string eventName;
		};

		/// <summary>
		/// �L�[�t���[���B
		/// </summary>
		struct KeyFrame {
			std::uint32_t boneIndex;	//�{�[���C���f�b�N�X�B
			float time;					//���ԁB
			Vector3 transform[4];		//�g�����X�t�H�[���B
		};
		/// <summary>
		/// �ǂݍ��ݏ����B
		/// </summary>
		/// <param name="filePath"></param>
		void Load(const char* filePath);
		/// <summary>
		/// �A�j���[�V�����C�x���g�ɃN�G�����s���B
		/// </summary>
		/// <param name="query">�N�G���֐�</param>
		void QueryAnimationEvents(std::function<void(const AnimationEvent& animEvent) > query)
		{
			for (auto& animEvent : m_animationEvents) {
				query(animEvent);
			}
		}
		/// <summary>
		/// �L�[�t���[���ɃN�G�����s���B
		/// </summary>
		/// <param name="query">�N�G���֐��B</param>
		void QueryKeyFrames(std::function<void(const KeyFrame& keyFrame)> query)
		{
			for (auto& keyFrame : m_keyFrames) {
				query(keyFrame);
			}
		}
		/// <summary>
		/// �A�j���[�V�����C�x���g�̐����擾�B
		/// </summary>
		/// <returns></returns>
		int GetNumAnimationEvent() const
		{
			return static_cast<int>(m_animationEvents.size());
		}
		/// <summary>
		/// �L�[�t���[���̐����擾�B
		/// </summary>
		/// <returns></returns>
		int GetNumKeyFrame() const
		{
			return static_cast<int>(m_keyFrames.size());
		}
	private:
		std::vector< AnimationEvent> m_animationEvents;	//�A�j���[�V�����C�x���g�B
		std::vector< KeyFrame> m_keyFrames;				//�L�[�t���[���B
	};
}