#include "k2EngineLowPreCompile.h"
#include "tkFile/TkaFile.h"
#include <errno.h>

namespace nsK2EngineLow {
	void TkaFile::Load(const char* filePath)
	{
		FILE* fp = fopen(filePath, "rb");
		if (fp == nullptr) {
			MessageBoxA(nullptr, "tka�t�@�C���̃I�[�v���Ɏ��s���܂����B", "�G���[", MB_OK);
			return;
		}
		//�A�j���[�V�����N���b�v�̃w�b�_�[�����[�h�B
		AnimClipHeader header;
		fread(&header, sizeof(header), 1, fp);

		if (header.numAnimationEvent > 0) {
			//�A�j���[�V�����C�x���g������Ȃ�A�C�x���g�������[�h����B
			for (auto i = 0; i < (int)header.numAnimationEvent; i++) {
				//�C�x���g���N�����鎞�Ԃ�ǂݍ��ށB
				float invokeTime = 0.0f;
				fread(&invokeTime, sizeof(invokeTime), 1, fp);
				//�C�����ƃ��C�̒�����ǂݍ��ށB
				std::uint32_t eventNameLength;
				fread(&eventNameLength, sizeof(eventNameLength), 1, fp);
				//�C�x���g�������[�h����B
				static char eventName[256];
				static wchar_t wEventName[256];
				fread(eventName, eventNameLength + 1, 1, fp);
				AnimationEvent animEvent;
				animEvent.invokeTime = invokeTime;
				animEvent.eventName = eventName;
				m_animationEvents.push_back(std::move(animEvent));
			}
		}
		//�L�[�t���[���̏����������ƃ��[�h�B
		m_keyFrames.resize(header.numKey);

		fread(&m_keyFrames.front(), sizeof(KeyFrame) * header.numKey, 1, fp);

		fclose(fp);

	}
}