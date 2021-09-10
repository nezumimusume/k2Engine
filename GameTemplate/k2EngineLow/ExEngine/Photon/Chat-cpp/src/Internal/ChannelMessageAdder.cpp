/* Exit Games Photon Chat - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "Chat-cpp/inc/Channel.h"
#include "Chat-cpp/inc/Internal/ChannelMessageAdder.h"

namespace ExitGames
{
	namespace Chat
	{
		namespace Internal
		{
			using namespace Common;

			void ChannelMessageAdder::add(Channel& channel, const JString& sender, const Object& message)
			{
				channel.add(sender, message);
			}

			void ChannelMessageAdder::add(Channel& channel, const JVector<JString>& senders, const JVector<Object>& messages)
			{
				channel.add(senders, messages);
			}
		}
	}
}