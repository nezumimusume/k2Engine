/* Exit Games Photon Chat - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "Chat-cpp/inc/Channel.h"

/** @file Channel.h */

namespace ExitGames
{
	namespace Chat
	{
		using namespace Common;
		using namespace Common::MemoryManagement;

		/**
		   @class Channel
		   Represents channel or private chat (channel with 2 users)*/

		Channel::Channel(const JString& name, int id, bool isPrivate)
			: mName(name)
			, mID(id)
			, mIsPrivate(isPrivate)
		{
		}

		const JString& Channel::getName(void) const
		{
			return mName;
		}

		const int Channel::getID(void) const
		{
			return mID;
		}

		unsigned int Channel::getMessageCount(void) const
		{
			return mMessages.getSize();
		}

		const JVector<JString>& Channel::getSenders(void) const
		{
			return mSenders;
		}

		const JVector<Object>& Channel::getMessages(void) const
		{
			return mMessages;
		}

		void Channel::add(const JString& sender, const Object& message)
		{
			mSenders.addElement(sender);
			mMessages.addElement(message);
		}

		void Channel::add(const JVector<JString>& senders, const JVector<Object>& messages)
		{
			for(unsigned int i=0; i<senders.getSize(); ++i)
				mSenders.addElement(senders[i]);
			for(unsigned int i=0; i<senders.getSize(); ++i)
				mMessages.addElement(messages[i]);
		}

		void Channel::readProperties(const Common::Dictionary<Common::Object, Common::Object>& props)
		{
			mCustomProperties.put(props);
			Common::Dictionary<Common::Object, Common::Object> stripped;
			const JVector<Object>& keys = mCustomProperties.getKeys();
			for(unsigned int i=0; i<mCustomProperties.getSize(); i++)
				if(mCustomProperties[i] != Object())
					stripped.put(keys[i], mCustomProperties[i]);
			mCustomProperties = stripped;
		}

		void Channel::clearMessages(void)
		{
			mSenders.removeAllElements();
			mMessages.removeAllElements();
		}

		bool Channel::getIsPrivate(void) const 
		{
			return mIsPrivate;
		}

		const Dictionary<Object, Object>& Channel::getCustomProperties() const
		{
			return mCustomProperties;
		}

		JString& Channel::toString(JString& retStr, bool withTypes) const
		{
			return retStr += L"ChatChannel " + mName;
		}
	}
}