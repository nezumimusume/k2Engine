/* Exit Games Photon Voice - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "PhotonVoice-cpp/inc/RemoteVoiceOptions.h"

/** @file PhotonVoice-cpp/inc/RemoteVoiceOptions.h */

namespace ExitGames
{
	namespace Voice
	{
		using namespace Common;
		using namespace Common::Helpers;
		using namespace Common::MemoryManagement;

		RemoteVoiceOptions::RemoteVoiceOptions()
			: mpRemoteVoiceRemoveActionOpaque(NULL)
			, mpRemoteVoiceRemoveAction(NULL)
			, mOutputType(OutputType::None)
			, mpOutputOpaque(NULL)
#if EG_PHOTON_VOICE_VIDEO_ENABLE
			, mOutputImageFormat(ImageFormat::Undefined) // value does not matter
			, mOutputImageFlip(Flip::Undefined) // value does not matter
#endif
			, mspDecoder()
		{
		}

		/// <summary>
		/// Register a method to be called when the remote voice is removed.
		/// </summary>
		void RemoteVoiceOptions::setRemoteVoiceRemoveAction(void* pOpaque, void(*pAction)(void*))
		{
			mpRemoteVoiceRemoveActionOpaque = pOpaque;
			mpRemoteVoiceRemoveAction = pAction;
		}

		void RemoteVoiceOptions::setOutput(void* pOpaque, void(*pOutput)(void*, const Buffer<float>&))
		{
			mpOutputOpaque = pOpaque;
			mOutputType = OutputType::Float;
			mOutput.mpFloatOutput = pOutput;
		}

		void RemoteVoiceOptions::setOutput(void* pOpaque, void(*pOutput)(void*, const Buffer<short>&))
		{
			mpOutputOpaque = pOpaque;
			mOutputType = OutputType::Short;
			mOutput.mpShortOutput = pOutput;
		}

#if EG_PHOTON_VOICE_VIDEO_ENABLE
		void RemoteVoiceOptions::setOutput(void* pOpaque, void(*pOutput)(void*, const ImageOutputBuf&))
		{
			mpOutputOpaque = pOpaque;
			mOutputType = OutputType::Image;
			mOutput.mpImageOutput = pOutput;
		}

		void RemoteVoiceOptions::setOutputImageFormat(ImageFormat::Enum value)
		{
			mOutputImageFormat = value;
		}

		void RemoteVoiceOptions::setOutputImageFlip(Flip::Enum value)
		{
			mOutputImageFlip = value;
		}
#endif

		void RemoteVoiceOptions::setDecoder(SharedPointer<IDecoder> decoder)
		{
			mspDecoder = decoder;
		}

		JString& RemoteVoiceOptions::toString(JString& retStr, bool /*withTypes*/) const
		{
			return retStr
				+= JString(L"{")
				+ L"outputType=" + mOutputType
#if EG_PHOTON_VOICE_VIDEO_ENABLE
				+ L" outputImageFormat=" + mOutputImageFormat
				+ L" outputImageFlip=" + mOutputImageFlip
#endif
				+ L" decoder=" + L"{" + Helpers::TypeName::get(mspDecoder) + L"}"
				+ L"}";
		}
	}
}