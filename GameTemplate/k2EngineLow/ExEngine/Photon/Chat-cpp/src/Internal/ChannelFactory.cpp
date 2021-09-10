/* Exit Games Photon Chat - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "Chat-cpp/inc/Channel.h"
#include "Chat-cpp/inc/Internal/ChannelFactory.h"

namespace ExitGames
{
	namespace Chat
	{
		namespace Internal
		{
			using namespace Common::MemoryManagement;

			Channel* ChannelFactory::create(const Common::JString& name, int id, bool isPrivate)
			{
				Channel* p;
				return ALLOCATE(Channel, p, name, id, isPrivate);
			}

			void ChannelFactory::destroy(const Channel* pChannel)
			{
				deallocate(pChannel);
			}
		}
	}
}