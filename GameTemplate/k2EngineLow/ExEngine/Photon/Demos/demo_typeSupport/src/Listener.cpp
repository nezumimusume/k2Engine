#include "Photon_lib.h"

//marco to disable warning -Wunused-but-set-variable
#define USED_VAR(type, name) type name = 0; if(name){}

extern UIListenerPtr gUIListener;

Listener::Listener(CPhotonLib& lib, ExitGames::Common::Logger& logger) 
#ifdef _EG_MS_COMPILER
#	pragma warning(push)
#	pragma warning(disable:4355)
#endif
: cPhotonLib(lib)
, mLogger(logger)
#ifdef _EG_MS_COMPILER
#	pragma warning(pop)
#endif
{
}

Listener::~Listener()
{
}

void Listener::debugReturn(int /*debugLevel*/, const ExitGames::Common::JString& string)
{
	gUIListener->writeString(ExitGames::Common::JString(L"debugReturn - ") + string + L"\n");
}

void Listener::connectionErrorReturn(int errorCode)
{
	EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	gUIListener->writeString(ExitGames::Common::JString(L"received connection error ") + errorCode);
	cPhotonLib.setState(CPhotonLib::Disconnected);
	gUIListener->writeString(L"\n-------DISCONNECTED-------\n");
}

void Listener::clientErrorReturn(int errorCode)
{
	EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	gUIListener->writeString(ExitGames::Common::JString(L"received error ") + errorCode + L" from client\n");
}

void Listener::warningReturn(int warningCode)
{
	EGLOG(ExitGames::Common::DebugLevel::WARNINGS, L"code: %d", warningCode);
	gUIListener->writeString(ExitGames::Common::JString(L"received warning ") + warningCode + L" from client\n");
}

void Listener::serverErrorReturn(int errorCode)
{
	EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	gUIListener->writeString(ExitGames::Common::JString(L"received error ") + errorCode + " from server\n");
}

void Listener::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& /*playernrs*/, const ExitGames::LoadBalancing::Player& player)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"%ls joined the game", player.getName().cstr());
	gUIListener->writeString(ExitGames::Common::JString(L"player ") + playerNr + L" " + player.getName() + L" has joined the game\n");
}

void Listener::leaveRoomEventAction(int playerNr, bool isInactive)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	gUIListener->writeString(ExitGames::Common::JString(L"player ") + playerNr + L" has left the game\n");
}

void Listener::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContentObj)
{
	gUIListener->writeString(ExitGames::Common::JString(L"\n") + eventContentObj.toString(true) + L"\n");

	switch(eventCode)
	{
	case 0:
		{
			const nByte POS_X = 101, POS_Y = 102, key2 = 103;
			float* ary = NULL;
			int* ary2 = NULL;
			short* aryLength3 = NULL;
			ExitGames::Common::Hashtable testHash;
			SampleCustomType sampleCustomType;
			SampleCustomType* array = NULL;
			SampleCustomType2 sampleCustomType2;
			ExitGames::Common::Object* objAry = NULL;
			USED_VAR(int, x);
			USED_VAR(int, y);
			USED_VAR(short, aryLength);
			USED_VAR(short, aryLength2);
			USED_VAR(short, arrayLength);
			USED_VAR(short, objAryLength);
			ExitGames::Common::Dictionary<int, ExitGames::Common::JString>*** ary3 = 0; if(ary3) {} //disable warning -Wunused-but-set-variable

			ExitGames::Common::Hashtable eventDataContent = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventContentObj).getDataCopy();

			// nByte key and int value:
			if(eventDataContent.getValue(POS_X))
				x = (ExitGames::Common::ValueObject<int>(eventDataContent.getValue(POS_X))).getDataCopy();

			// ExitGames::Common::JString key and float-array value: - getting the array and it's length
			if(eventDataContent.getValue(L"testKey"))
				ary = (ExitGames::Common::ValueObject<float*>(eventDataContent.getValue(L"testKey"))).getDataCopy();
			if(eventDataContent.getValue(L"testKey"))
				aryLength = *(ExitGames::Common::ValueObject<float*>(eventDataContent.getValue(L"testKey"))).getSizes();

			// nByte key and ExitGames::Common::Hashtable value
			if(eventDataContent.getValue(key2))
			{
				testHash = (ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventDataContent.getValue(key2))).getDataCopy();

				if(testHash.getValue(POS_X))
					x = (ExitGames::Common::ValueObject<int>(testHash.getValue(POS_X))).getDataCopy();
				if(testHash.getValue(POS_Y))
					y = (ExitGames::Common::ValueObject<int>(testHash.getValue(POS_Y))).getDataCopy();
			}

			// ExitGames::Common::JString key and empty int-array value:
			if(eventDataContent.getValue(L"emptyArray"))
				ary2 = (ExitGames::Common::ValueObject<int*>(eventDataContent.getValue(L"emptyArray"))).getDataCopy();
			if(eventDataContent.getValue(L"emptyArray"))
				aryLength2 = *(ExitGames::Common::ValueObject<int*>(eventDataContent.getValue(L"emptyArray"))).getSizes();

			// ExitGames::Common::JString key and 3D ExitGames::Common::Dictionary-array value:
			ExitGames::Common::ValueObject<ExitGames::Common::Dictionary<int, ExitGames::Common::JString>***>* pTemp = 0;
			if(eventDataContent.getValue(L"array3d"))
			{
				pTemp = new ExitGames::Common::ValueObject<ExitGames::Common::Dictionary<int, ExitGames::Common::JString>***>(eventDataContent.getValue(L"array3d"));
				ary3 = *(*pTemp).getDataAddress();
				aryLength3 = new short[(*pTemp).getDimensions()];
				for(unsigned int i=0; i<(*pTemp).getDimensions(); i++)
					aryLength3[i] = (*pTemp).getSizes()[i];
			}

			// ExitGames::Common::JString key and SampleCustomType value:
			if(eventDataContent.getValue(L"customType"))
				sampleCustomType = (ExitGames::Common::ValueObject<SampleCustomType>(eventDataContent.getValue(L"customType"))).getDataCopy();

			// ExitGames::Common::JString key and SampleCustomType-array value:
			if(eventDataContent.getValue(L"array"))
				array = (ExitGames::Common::ValueObject<SampleCustomType*>(eventDataContent.getValue(L"array"))).getDataCopy();
			if(eventDataContent.getValue(L"array"))
				arrayLength = *(ExitGames::Common::ValueObject<SampleCustomType*>(eventDataContent.getValue(L"array"))).getSizes();

			// ExitGames::Common::JString key and SampleCustomType2 value:
			if(eventDataContent.getValue(L"customType2"))
				sampleCustomType2 = (ExitGames::Common::ValueObject<SampleCustomType2>(eventDataContent.getValue(L"customType2"))).getDataCopy();

			// ExitGames::Common::JString key and ExitGames::Common::Object-array value
			if(eventDataContent.getValue(L"objectArray"))
				objAry = (ExitGames::Common::ValueObject<ExitGames::Common::Object*>(*eventDataContent.getValue(L"objectArray"))).getDataCopy();
			if(eventDataContent.getValue(L"objectArray"))
				objAryLength = *(ExitGames::Common::ValueObject<ExitGames::Common::Object*>(*eventDataContent.getValue(L"objectArray"))).getSizes();

			ExitGames::Common::MemoryManagement::deallocateArray(ary);
			ExitGames::Common::MemoryManagement::deallocateArray(ary2);
			ExitGames::Common::MemoryManagement::deallocateArray(array);
			ExitGames::Common::MemoryManagement::deallocateArray(objAry);
			delete[] aryLength3;
			delete pTemp;
		}
		break;
	default:
		break;
	}
}

void Listener::connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"connected to cluster " + cluster + L" of region " + region);
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		cPhotonLib.setState(CPhotonLib::ErrorConnecting);
		return;
	}
	cPhotonLib.setState(CPhotonLib::Connected);
	gUIListener->writeString(L"\n-------CONNECTED-------\n");
}

void Listener::disconnectReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	cPhotonLib.setState(CPhotonLib::Disconnected);
	gUIListener->writeString(L"\n-------DISCONNECTED-------\n");
}

void Listener::createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		gUIListener->writeString(L"opCreateRoom() failed: \n" + errorString);
		//cPhotonLib.setState(CPhotonLib::Connected);
		cPhotonLib.setState(CPhotonLib::ErrorJoining);
		return;
	}

	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	gUIListener->writeString(L"... room " + cPhotonLib.getClient().getCurrentlyJoinedRoom().getName() + " has been created\n");
	gUIListener->writeString(L"regularly sending dummy events now\n");
	
	cPhotonLib.setState(CPhotonLib::Joined);
	gUIListener->writeString(L"\n----------JOINED-------\n");
}

void Listener::joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		gUIListener->writeString(L"opJoinOrCreateRoom() failed: \n" + errorString);
		//cPhotonLib.setState(CPhotonLib::Connected);
		cPhotonLib.setState(CPhotonLib::ErrorJoining);
		return;
	}

	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	gUIListener->writeString(L"... room " + cPhotonLib.getClient().getCurrentlyJoinedRoom().getName() + " has been entered\n");
	gUIListener->writeString(L"regularly sending dummy events now\n");

	cPhotonLib.setState(CPhotonLib::Joined);
	gUIListener->writeString(L"\n----------JOINED-------\n");
}

void Listener::joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{		
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		gUIListener->writeString(L"opJoinRoom() failed: \n" + errorString);
		//cPhotonLib.setState(CPhotonLib::Connected);
		cPhotonLib.setState(CPhotonLib::ErrorJoining);
		return;
	}
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	gUIListener->writeString(L"... room " + cPhotonLib.getClient().getCurrentlyJoinedRoom().getName() + " has been successfully joined\n");
	gUIListener->writeString(L"regularly sending dummy events now\n");

	cPhotonLib.setState(CPhotonLib::Joined);
	gUIListener->writeString(L"\n----------JOINED-------\n");
}

void Listener::joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		gUIListener->writeString(L"opJoinRandomRoom() failed: \n" + errorString);
		//cPhotonLib.setState(CPhotonLib::Connected);
		cPhotonLib.setState(CPhotonLib::ErrorJoining);
		return;
	}

	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);	
	gUIListener->writeString(L"... room " + cPhotonLib.getClient().getCurrentlyJoinedRoom().getName() + " has been successfully joined\n");
	gUIListener->writeString(L"regularly sending dummy events now\n");

	cPhotonLib.setState(CPhotonLib::Joined);
	gUIListener->writeString(L"\n----------JOINED-------\n");
}

void Listener::leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		gUIListener->writeString(L"opLeaveRoom() failed: \n" + errorString);
		//cPhotonLib.setState(CPhotonLib::Disconnecting);
		cPhotonLib.setState(CPhotonLib::ErrorLeaving);
		return;
	}
	cPhotonLib.setState(CPhotonLib::Left);
	gUIListener->writeString(L"\n----------LEFT-------\n");
}

void Listener::joinLobbyReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	gUIListener->writeString(L"joined lobby\n");
}

void Listener::leaveLobbyReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	gUIListener->writeString(L"left lobby\n");
}