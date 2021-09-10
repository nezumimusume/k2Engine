/* Exit Games Photon Voice - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "PhotonVoice-cpp/inc/Buffer.h"

/** @file PhotonVoice-cpp/inc/Buffer.h */

namespace ExitGames
{
	namespace Voice
	{
		template class Buffer<float>;
		template class Buffer<short>;
		template class Buffer<nByte>;
	}
}