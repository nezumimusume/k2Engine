/* Exit Games Photon Voice - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "PhotonVoice-cpp/inc/VoiceCodec.h"

/** @file PhotonVoice-cpp/inc/VoiceCodec.h */

namespace ExitGames
{
	namespace Voice
	{
		using namespace Common;

		UnsupportedCodecError::UnsupportedCodecError(const JString& info, int codec, const ILogger& logger)
		{
			mError = JString("[PV] ") + info + L": unsupported codec " + codec;
			logger.logError(mError);
		}

		JString UnsupportedCodecError::getError(void) const
		{
			return mError;
		}

		void UnsupportedCodecError::open(const VoiceInfo& /*info*/)
		{
		}

		void UnsupportedCodecError::setOutput(void* /*opaque*/, void(* /*output*/)(void*, const Buffer<nByte>&))
		{
		}

		Buffer<nByte> UnsupportedCodecError::dequeueOutput(void)
		{
			return Buffer<nByte>();
		}

		void UnsupportedCodecError::input(const Buffer<nByte>& /*buf*/)
		{
		}
	}
}