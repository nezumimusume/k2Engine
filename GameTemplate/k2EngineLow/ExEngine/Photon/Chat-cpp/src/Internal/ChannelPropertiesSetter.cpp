/* Exit Games Photon Chat - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "Chat-cpp/inc/Channel.h"
#include "Chat-cpp/inc/Internal/ChannelPropertiesSetter.h"

namespace ExitGames
{
	namespace Chat
	{
		namespace Internal
		{
			using namespace Common;

			void ChannelPropertiesSetter::readProperties(Channel& channel, const Common::Dictionary<Common::Object, Common::Object>& props)
			{
				channel.readProperties(props);
			}
		}
	}
}