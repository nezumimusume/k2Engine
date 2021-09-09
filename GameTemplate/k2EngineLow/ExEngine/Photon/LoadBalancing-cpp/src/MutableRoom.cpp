/* Exit Games Photon LoadBalancing - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "LoadBalancing-cpp/inc/Client.h"
#include "LoadBalancing-cpp/inc/MutableRoom.h"
#include "LoadBalancing-cpp/inc/Internal/PlayerMutableRoomPointerSetter.h"
#include "LoadBalancing-cpp/inc/Internal/Utils.h"
#include "LoadBalancing-cpp/inc/Internal/Enums/Properties/Room.h"

/** @file inc/MutableRoom.h */

namespace ExitGames
{
	namespace LoadBalancing
	{
		using namespace Common;
		using namespace Common::MemoryManagement;
		using namespace Internal;

		MutableRoom::MutableRoom(const JString& name, const Hashtable& properties, Client* pClient, const JVector<JString>& propsListedInLobby, int playerTtl, int emptyRoomTtl, bool suppressRoomEvents, const JVector<JString>* pPlugins, bool publishUserID, const JVector<JString>& expectedUsers)
			: super(name, properties)
			, mpLoadBalancingClient(pClient)
			, mIsVisible(true)
			, mPlayers(JVector<Player*>())
			, mMasterClientID(0)
			, mPropsListedInLobby(propsListedInLobby)
			, mLocalPlayerNumber(-1)
			, mPlayerTtl(playerTtl)
			, mEmptyRoomTtl(emptyRoomTtl)
			, mSuppressRoomEvents(suppressRoomEvents)
			, mupPlugins(pPlugins?allocate<JVector<JString> >(*pPlugins):NULL)
			, mPublishUserID(publishUserID)
			, mExpectedUsers(expectedUsers)
		{
			cacheProperties(properties);
		}

		MutableRoom::~MutableRoom(void)
		{
			destroyAllPlayers();
		}

		MutableRoom::MutableRoom(const MutableRoom& toCopy)
			: super(toCopy)
		{
			*this = toCopy;
		}

		MutableRoom& MutableRoom::operator=(const Room& toCopy)
		{
			return assign(toCopy);
		}

		MutableRoom& MutableRoom::operator=(const MutableRoom& toCopy)
		{
			return assign(toCopy);
		}

		bool MutableRoom::getIsMutable(void) const
		{
			return true;
		}

		MutableRoom& MutableRoom::assign(const Room& toCopy)
		{
			super::assign(toCopy);
			const MutableRoom& temp = static_cast<const MutableRoom&>(toCopy);
			if(!temp.getIsMutable())
				return *this;
			mpLoadBalancingClient = temp.mpLoadBalancingClient;
			mIsVisible = temp.mIsVisible;
			mPlayers.removeAllElements();
			for(unsigned int i=0; i<temp.mPlayers.getSize(); ++i)
			{
				mPlayers.addElement(temp.mPlayers[i]->getNumber()==temp.mLocalPlayerNumber?temp.mPlayers[i]:allocate<Player>(*temp.mPlayers[i]));
				PlayerMutableRoomPointerSetter::setMutableRoomPointer(*mPlayers[i], this);
			}
			mMasterClientID = temp.mMasterClientID;
			mPropsListedInLobby = temp.mPropsListedInLobby;
			mLocalPlayerNumber = temp.mLocalPlayerNumber;
			mPlayerTtl = temp.mPlayerTtl;
			mEmptyRoomTtl = temp.mEmptyRoomTtl;
			mSuppressRoomEvents = temp.mSuppressRoomEvents;
			mupPlugins = temp.mupPlugins?allocate<JVector<JString> >(*temp.mupPlugins):NULL;
			mPublishUserID = temp.mPublishUserID;
			mExpectedUsers = temp.mExpectedUsers;
			return *this;
		}

		bool MutableRoom::mergeCustomProperties(const Hashtable& customProperties, const Hashtable& expectedCustomProperties, const WebFlags& webflags)
		{
			Hashtable stripDict = Utils::stripToCustomProperties(customProperties);
			if(!stripDict.getSize())
				return false;
			Hashtable mergeResult = mCustomProperties;
			mergeResult.put(stripDict);
			mergeResult = Utils::stripKeysWithNullValues(mergeResult);
			if(mCustomProperties == mergeResult)
				return false;
			bool retVal = mpLoadBalancingClient->opSetPropertiesOfRoom(stripDict, expectedCustomProperties, webflags);
			if(retVal && !expectedCustomProperties.getSize())
				mCustomProperties = mergeResult;
			return retVal;
		}

		bool MutableRoom::addCustomProperties(const Hashtable& customProperties, const Hashtable& expectedCustomProperties, const WebFlags& webflags)
		{
			return mergeCustomProperties(Utils::stripKeysWithNullValues(customProperties), expectedCustomProperties, webflags);
		}

		void MutableRoom::cacheProperties(const Hashtable& properties)
		{
			if(properties.contains(Properties::Room::IS_VISIBLE))
				mIsVisible = ValueObject<bool>(properties.getValue(Properties::Room::IS_VISIBLE)).getDataCopy();
			if(properties.contains(Properties::Room::EXPECTED_USERS))
			{
				ValueObject<JString*> obj = ValueObject<JString*>(properties.getValue(Properties::Room::EXPECTED_USERS));
				mExpectedUsers = (obj.getType() == TypeCode::EG_NULL)?JVector<JString>():JVector<JString>(*obj.getDataAddress(), *obj.getSizes());
			}
			if(properties.contains(Properties::Room::MASTER_CLIENT_ID))
				setMasterClientID(ValueObject<int>(properties.getValue(Properties::Room::MASTER_CLIENT_ID)).getDataCopy());
			if(properties.contains(Properties::Room::PROPS_LISTED_IN_LOBBY))
			{
				ValueObject<JString*> obj(properties.getValue(Properties::Room::PROPS_LISTED_IN_LOBBY));
				mPropsListedInLobby = JVector<JString>(*obj.getDataAddress(), *obj.getSizes());
			}
			super::cacheProperties(properties);
		}

		JString MutableRoom::toString(bool withTypes, bool withCustomProperties, bool withPlayers) const
		{
			return mName + L"={" + payloadToString(withTypes, withCustomProperties, withPlayers) + L"}";
		}

		JString MutableRoom::payloadToString(bool withTypes, bool withCustomProperties, bool withPlayers) const
		{
			JString res = JString(L"masterClient: ") + mMasterClientID + L" " + super::payloadToString() + L" visible: " + mIsVisible + L" expectedUsers: " + mExpectedUsers.toString();
			if(mPropsListedInLobby.getSize())
				res += L" propsListedInLobby: " + mPropsListedInLobby.toString(withTypes);
			if(withCustomProperties && mCustomProperties.getSize())
				res += L" props: " + mCustomProperties.toString(withTypes);
			if(withPlayers && mPlayers.getSize())
			{
				res += L" players: ";
				for(unsigned int i=0; i<mPlayers.getSize(); ++i)
					res += mPlayers[i]->toString(withTypes, withCustomProperties) + (i<mPlayers.getSize()-1?L", ":L"");
			}
			return res;
		}

		nByte MutableRoom::getPlayerCount(void) const
		{
			return mPlayers.getSize();
		}

		bool MutableRoom::setMaxPlayers(nByte maxPlayers, const WebFlags& webflags)
		{
			if(mMaxPlayers == maxPlayers)
				return false;
			bool retVal = setRoomProperty(Properties::Room::MAX_PLAYERS, maxPlayers, webflags);
			if(retVal)
				mMaxPlayers = maxPlayers;
			return retVal;
		}

		bool MutableRoom::setIsOpen(bool isOpen, const WebFlags& webflags)
		{
			if(mIsOpen == isOpen)
				return false;
			bool retVal = setRoomProperty(Properties::Room::IS_OPEN, isOpen, webflags);
			if(retVal)
				mIsOpen = isOpen;
			return retVal;
		}

		bool MutableRoom::getIsVisible(void) const
		{
			return mIsVisible;
		}

		bool MutableRoom::setIsVisible(bool isVisible, const WebFlags& webflags)
		{
			if(mIsVisible == isVisible)
				return false;
			bool retVal = setRoomProperty(Properties::Room::IS_VISIBLE, isVisible, webflags);
			if(retVal)
				mIsVisible = isVisible;
			return retVal;
		}

		template<typename Etype> bool MutableRoom::setRoomProperty(nByte key, Etype val, const WebFlags& webflags)
		{
			Hashtable properties;
			properties.put(key, val);
			return mpLoadBalancingClient->opSetPropertiesOfRoom(properties, Hashtable(), webflags);
		}

		template<typename Etype> bool MutableRoom::setRoomProperty(nByte key, const Etype pValueArray, typename Common::Helpers::ArrayLengthType<Etype>::type arrSize, const WebFlags& webflags)
		{
			Hashtable properties;
			properties.put(key, pValueArray, arrSize);
			return mpLoadBalancingClient->opSetPropertiesOfRoom(properties, Hashtable(), webflags);
		}

		template<typename Etype> bool MutableRoom::setRoomProperty(nByte key, const Etype pValueArray, const short* pArrSizes, const WebFlags& webflags)
		{
			Hashtable properties;
			properties.put(key, pValueArray, pArrSizes);
			return mpLoadBalancingClient->opSetPropertiesOfRoom(properties, Hashtable(), webflags);
		}

		const JVector<Player*>& MutableRoom::getPlayers(void) const
		{
			return mPlayers;
		}
		
		const Player* MutableRoom::getPlayerForNumber(int playerNumber) const
		{
			for(unsigned i=0; i<mPlayers.getSize(); i++)
				if(mPlayers[i]->getNumber() == playerNumber)
					return mPlayers[i];
			return NULL;
		}

		int MutableRoom::getMasterClientID(void) const
		{
			return mMasterClientID;
		}

		bool MutableRoom::setMasterClientID(int id)
		{			
			if(mMasterClientID != id)
			{
				int oldID = mMasterClientID;
				mMasterClientID = id;
				mpLoadBalancingClient->onMasterClientChanged(mMasterClientID, oldID);
				return true;
			}
			else
				return false;
		}

		const JVector<JString>& MutableRoom::getPropsListedInLobby(void) const
		{
			return mPropsListedInLobby;
		}

		bool MutableRoom::setPropsListedInLobby(const JVector<JString>& propsListedInLobby, const JVector<JString>& expectedList, const WebFlags& webflags)
		{
			if(mPropsListedInLobby == propsListedInLobby)
				return false;
			Hashtable properties, expectedProperties;
			properties.put(Properties::Room::PROPS_LISTED_IN_LOBBY, propsListedInLobby.getCArray(), propsListedInLobby.getSize());
			if(expectedList.getSize())
				expectedProperties.put(Properties::Room::PROPS_LISTED_IN_LOBBY, expectedList.getCArray(), expectedList.getSize());
			bool retVal = mpLoadBalancingClient->opSetPropertiesOfRoom(properties, expectedProperties, webflags);
			if(retVal && !expectedList.getSize())
				mPropsListedInLobby = propsListedInLobby;
			return retVal;
		}

		int MutableRoom::getPlayerTtl(void) const
		{
			return mPlayerTtl;
		}

		int MutableRoom::getEmptyRoomTtl(void) const
		{
			return mEmptyRoomTtl;
		}
		bool MutableRoom::getSuppressRoomEvents(void) const
		{
			return mSuppressRoomEvents;
		}

		const Common::JVector<Common::JString>* MutableRoom::getPlugins(void) const
		{
			return mupPlugins;
		}

		bool MutableRoom::getPublishUserID(void) const
		{
			return mPublishUserID;
		}

		const JVector<JString>& MutableRoom::getExpectedUsers(void) const
		{
			return mExpectedUsers;
		}

		bool MutableRoom::setExpectedUsers(const Common::JVector<Common::JString>& expectedUsers, const WebFlags& webflags)
		{
			if(mExpectedUsers == expectedUsers)
				return false;
			Hashtable properties, expectedProperties;
			if(expectedUsers.getSize())
				properties.put(Properties::Room::EXPECTED_USERS, expectedUsers.getCArray(), expectedUsers.getSize());
			else
				properties.put(Properties::Room::EXPECTED_USERS);
			if(getExpectedUsers().getSize())
				expectedProperties.put(Properties::Room::EXPECTED_USERS, getExpectedUsers().getCArray(), getExpectedUsers().getSize());
			else
				expectedProperties.put(Properties::Room::EXPECTED_USERS);
			return mpLoadBalancingClient->opSetPropertiesOfRoom(properties, expectedProperties, webflags);
		}

		Player* MutableRoom::createPlayer(int number, const Hashtable& properties) const
		{
			return PlayerFactory::create(number, properties, this);
		}

		void MutableRoom::destroyPlayer(const Player* pPlayer) const
		{
			PlayerFactory::destroy(pPlayer);
		}

		void MutableRoom::setPlayers(const JVector<Player*>& players)
		{
			mPlayers = players;
		}

		void MutableRoom::removeAllPlayers(void)
		{
			destroyAllPlayers();
			mPlayers.removeAllElements();
		}

		void MutableRoom::destroyAllPlayers(void)
		{
			for(unsigned int i=0; i<mPlayers.getSize(); ++i)
				if(mPlayers[i]->getNumber() != mLocalPlayerNumber)
					destroyPlayer(mPlayers[i]);
		}
		
		void MutableRoom::addPlayer(Player& player)
		{
			mPlayers.addElement(&player);
		}
		
		void MutableRoom::addLocalPlayer(Player& player)
		{
			mLocalPlayerNumber = player.getNumber();
			addPlayer(player);
		}
		
		void MutableRoom::addPlayer(int number, const Common::Hashtable& properties)
		{
			addPlayer(*createPlayer(number, properties));
		}
		
		bool MutableRoom::removePlayer(int number)
		{
			bool retVal = false;
			for(unsigned int i=0; i<mPlayers.getSize(); ++i)
			{
				if(mPlayers[i]->getNumber() == number)
				{
					destroyPlayer(mPlayers[i]);
					mPlayers.removeElementAt(i);
					retVal = true;
				}
			}
			return retVal;
		}

		JVector<Player*>& MutableRoom::getNonConstPlayers(void)
		{
			return mPlayers;
		}
	}
}