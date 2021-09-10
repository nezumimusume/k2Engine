/* Exit Games Photon Voice - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "PhotonVoice-cpp/inc/Transport/LoadBalancingListener.h"
#include "PhotonVoice-cpp/inc/Transport/LoadBalancingTransport.h"

/** @file PhotonVoice-cpp/inc/Transport/LoadBalancingListener.h */

namespace ExitGames
{
	namespace Voice
	{
		using namespace Common;
		using namespace Common::MemoryManagement;

		// functions
		LoadBalancingListener::LoadBalancingListener(void)
			: mpTransport(NULL)
		{
		}

		void LoadBalancingListener::createRoomReturn(int /*localPlayerNr*/, const Hashtable& /*roomProperties*/, const Hashtable& /*playerProperties*/, int /*errorCode*/, const JString& /*errorString*/)
		{
			mpTransport->onJoinRoom();
		}

		void LoadBalancingListener::joinOrCreateRoomReturn(int /*localPlayerNr*/, const Hashtable& /*roomProperties*/, const Hashtable& /*playerProperties*/, int /*errorCode*/, const JString& /*errorString*/)
		{
			mpTransport->onJoinRoom();
		}

		void LoadBalancingListener::joinRandomOrCreateRoomReturn(int /*localPlayerNr*/, const Hashtable& /*roomProperties*/, const Hashtable& /*playerProperties*/, int /*errorCode*/, const JString& /*errorString*/)
		{
			mpTransport->onJoinRoom();
		}

		void LoadBalancingListener::joinRoomReturn(int /*localPlayerNr*/, const Hashtable& /*roomProperties*/, const Hashtable& /*playerProperties*/, int /*errorCode*/, const JString& /*errorString*/)
		{
			mpTransport->onJoinRoom();
		}

		void LoadBalancingListener::joinRandomRoomReturn(int /*localPlayerNr*/, const Hashtable& /*roomProperties*/, const Hashtable& /*playerProperties*/, int /*errorCode*/, const JString& /*errorString*/)
		{
			mpTransport->onJoinRoom();
		}

		void LoadBalancingListener::leaveRoomReturn(int /*errorCode*/, const JString& /*errorString*/)
		{
			mpTransport->onLeaveRoom();
		}

		void LoadBalancingListener::disconnectReturn(void)
		{
			mpTransport->onLeaveRoom();
		}

		void LoadBalancingListener::joinRoomEventAction(int playerNr, const JVector<int>& /*playernrs*/, const LoadBalancing::Player& /*player*/)
		{
			mpTransport->onPlayerJoinRoom(playerNr);
		}

		void LoadBalancingListener::leaveRoomEventAction(int playerNr, bool /*isInactive*/)
		{
			mpTransport->onPlayerLeaveRoom(playerNr);
		}

		void LoadBalancingListener::customEventAction(int playerNr, nByte eventCode, const Object& eventContent)
		{
			mpTransport->onCustomEvent(playerNr, eventCode, eventContent);
		}		

		void LoadBalancingListener::onDirectMessage(const Object& msg, int remoteID, bool relay)
		{
			mpTransport->onDirectMessage(msg, remoteID, relay);
		}
	}
}