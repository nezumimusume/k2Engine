#include "Photon_lib.h"

static const ExitGames::Common::JString appID = L"<no-app-id>"; // set your app id here
static const ExitGames::Common::JString appVersion = L"1.0";

static const ExitGames::Common::JString PLAYER_NAME = L"user";

class Listener;
extern UIListenerPtr gUIListener;

CPhotonLib::CPhotonLib(void)
#ifdef _EG_MS_COMPILER
#	pragma warning(push)
#	pragma warning(disable:4355)
#endif
: mListener(*this, mLogger)
, mClient(mListener, appID, appVersion, ExitGames::Photon::ConnectionProtocol::DEFAULT, true)
#ifdef _EG_MS_COMPILER
#	pragma warning(pop)
#endif
{
	mClient.setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // that instance of LoadBalancingClient and its implementation details
	mLogger.setListener(mListener);
	mLogger.setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // this class
	ExitGames::Common::Base::setListener(&mListener);
	ExitGames::Common::Base::setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // all classes that inherit from Base


	gUIListener->writeString(L"Initialize ExitGames library\n");
	m_currentState = PhotonPeerCreated;
	SampleCustomType::registerType();
	SampleCustomType2::registerType();
}

CPhotonLib::~CPhotonLib(void)
{
	SampleCustomType::unregisterType();
	SampleCustomType2::unregisterType();
}

ExitGames::LoadBalancing::Client& CPhotonLib::getClient()
{
	return mClient;
}

int CPhotonLib::createConnection()
{
	mClient.connect(ExitGames::LoadBalancing::ConnectOptions().setAuthenticationValues(ExitGames::LoadBalancing::AuthenticationValues().setUserID(ExitGames::Common::JString()+GETTIMEMS())).setUsername(PLAYER_NAME+GETTIMEMS()));
	m_currentState = Connecting;
	return 0;
}

int CPhotonLib::closeConnection()
{
	mClient.disconnect();
	m_currentState = Disconnecting;
	return 0;
}

short CPhotonLib::join(const char* gameId)
{
	return mClient.opJoinOrCreateRoom(gameId);
}

short CPhotonLib::leave(void)
{
	return mClient.opLeaveRoom();
}

int CPhotonLib::run()
{
	bool bRun = true;
	char gameId[] = "demo_photon_game";

	while(bRun)
	{
		if(gUIListener->anyKeyPressed())
			bRun = false;

		mClient.service(); // Call this regularly (e.g. once per gameloop) to transmit generated commands and to check for server responses and new events.

		switch(m_currentState)
		{
		case PhotonPeerCreated:
			gUIListener->writeString(L"\n-------CONNECTING-------\n");
			createConnection();
			break;
		case Connecting:
			// Waiting for callback function
			break;
		case ErrorConnecting:
			// Stop run cycle
			gUIListener->writeString(L"\nErrorConnecting\n");
			bRun = false;
			break;
		case Connected:
			gUIListener->writeString(L"\n-------JOINING-------\n");
			m_currentState = Joining;
			if(join(gameId) == -1)
				m_currentState = ErrorJoining;
			break;
		case Joining:
			// Waiting for callback function
			break;
		case ErrorJoining:
			// Stop run cycle
			bRun = false;
			gUIListener->writeString(L"ErrorJoining\n");
			break;
		case Joined :
			m_currentState = Receiving;
			gUIListener->writeString(L"\n-------SENDING/RECEIVING DATA-------\n");
			break;
		case Receiving:
			sendData();
			break;
		case Leaving:
			// Waiting for callback function
			break;
		case ErrorLeaving:
			// Stop run cycle
			bRun = false;
			break;
		case Left:
			m_currentState = Disconnecting;
			gUIListener->writeString(L"\n-------DISCONNECTING-------\n");
			closeConnection();
			break;
		case Disconnecting:
			// Waiting for callback function
			break;
		case Disconnected:
			// Stop run cycle
			bRun = false;
			break;
		default:
			break;
		}
		SLEEP(100);
	}
	if(m_currentState >= Connected && m_currentState < Disconnecting)
	{
		m_currentState = Disconnecting;
		closeConnection();
	}

	return 0;
}

CPhotonLib::States CPhotonLib::getState() const
{
	return m_currentState;
}

void CPhotonLib::setState(CPhotonLib::States new_state)
{
	m_currentState = new_state;
}

void CPhotonLib::sendData(void)
{
	// this function sends some example data to demonstrate, how to send the different types of data, that are supported by Photon
	// have a look at Listener::customEventAction(), to see, how to read them out
	ExitGames::Common::Hashtable ev;

	// nByte key and int value:
	const nByte POS_X = 101;
	int x = 10;
	ev.put(POS_X, x);


	// JString key and float-array value:
	float pVal[10];
	for(int i=0; i<10; i++)
		pVal[i] = 1.1f*i;
	ev.put(L"testKey", pVal, 10);


	// nByte key and Hashtable value:
	ExitGames::Common::Hashtable testHash;
	const nByte POS_Y = 102;
	const nByte key2 = 103;
	int y = 20;
	testHash.put(POS_X, x);
	testHash.put(POS_Y, y);
	ev.put(key2, testHash);


	// JString key and empty int-array value:
	ev.put(L"emptyArray", static_cast<int*>(NULL), (short)0);


	// JString key and 3D Dictionary<int, JString>-array value
	const short size0 = 3;
	const short size1 = 3;
	const short size2 = 3;

	ExitGames::Common::Dictionary<int, ExitGames::Common::JString>** array3d[size0];
	for(short i=0; i<size0; i++)
	{
		array3d[i] = new ExitGames::Common::Dictionary<int, ExitGames::Common::JString>*[size1];
		for(short j=0; j<size1; j++)
		{
			array3d[i][j] = new ExitGames::Common::Dictionary<int, ExitGames::Common::JString>[size2];
			for(short k=0; k<size2; k++)
				array3d[i][j][k].put(100*i+10*j+k, ExitGames::Common::JString()+(100*i+10*j+k));
		}
	}

	const short size[3] = {size0, size1, size2};

	ev.put(L"array3d", array3d, size);
	for(int i=0; i<size0; i++)
	{
		for(int j=0; j<size1; j++)
			delete[] array3d[i][j];
		delete[] array3d[i];
	}


	// short key and Null value
	ev.put(static_cast<short>(1));


	// even custom types are possible
	ev.put(L"customType", SampleCustomType(12, 144));

	// and so are arrays of them, of course
	const short size3 = 3;

	SampleCustomType array[size3];
	for(short i=0; i<size3; i++)
		array[i] = SampleCustomType((i+2)*(i+2), (i+2)*(i+2)*(i+2));

	ev.put(L"array", array, size3);

	// custom types also can have Photon containers as members that themselves can hold other custom types
	ExitGames::Common::Dictionary<ExitGames::Common::JString, SampleCustomType> dic;
	dic.put(L"customTypeDictionaryEntry", SampleCustomType(9, 27));
	ev.put(L"customType2", SampleCustomType2(dic));


	// JString key and Dictionary<int, Dictionary<int, int> > value
	ExitGames::Common::Dictionary<int, ExitGames::Common::Dictionary<int, int> > outerDic;
	ExitGames::Common::Dictionary<int, int> innerDic;
	int test = 3;
	innerDic.put(2, test);
	outerDic.put(1, innerDic);
	ev.put(L"dic", outerDic);


	// JString key and Object-array value, filled with a colorful bouquet of different datatypes:
	ExitGames::Common::Object objectArray[8];
	objectArray[0] = ExitGames::Common::ValueObject<nByte>(10);
	objectArray[1] = ExitGames::Common::ValueObject<ExitGames::Common::JString>(L"string");
	objectArray[2] = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(ExitGames::Common::Hashtable());
	objectArray[3] = ExitGames::Common::ValueObject<float*>(pVal, 10);
	objectArray[4] = ExitGames::Common::ValueObject<int*>(NULL, (short)0);
	objectArray[5] = ExitGames::Common::ValueObject<ExitGames::Common::Object*>(objectArray, 5);
	objectArray[6] = ExitGames::Common::ValueObject<SampleCustomType>(array[0]);
	objectArray[7] = ExitGames::Common::ValueObject<ExitGames::Common::Dictionary<int, ExitGames::Common::Dictionary<int, int> > >(outerDic);
	ev.put(L"objectArray", objectArray, 8);

	// now that we filled our hashtable with some data, we will transmit it to all other players in the same game by calling opRaiseEvent().
	mClient.opRaiseEvent(false, ev, 0);
}



SampleCustomType::SampleCustomType(void)
{
}

SampleCustomType::SampleCustomType(nByte foo, nByte bar)
{
	mFoo = foo;
	mBar = bar;
}

SampleCustomType::SampleCustomType(const SampleCustomType& toCopy)
	: super()
{
	mFoo = toCopy.mFoo;
	mBar = toCopy.mBar;
}

SampleCustomType::~SampleCustomType(void)
{
}

SampleCustomType& SampleCustomType::operator=(const SampleCustomType& toCopy)
{
	mFoo = toCopy.mFoo;
	mBar = toCopy.mBar;
	return *this;
}

bool SampleCustomType::compare(const ExitGames::Common::CustomTypeBase& other) const
{
	return mFoo == static_cast<const SampleCustomType&>(other).mFoo && mBar == static_cast<const SampleCustomType&>(other).mBar;
}

void SampleCustomType::duplicate(ExitGames::Common::CustomTypeBase* pRetVal) const
{
	*reinterpret_cast<SampleCustomType*>(pRetVal) = *this;
}

void SampleCustomType::deserialize(const nByte* pData, short length)
{
	if(length!=2)
		return; // something went wrong
	mFoo = pData[0];
	mBar = pData[1];
}

short SampleCustomType::serialize(nByte* pRetVal) const
{
	if(pRetVal)
	{
		pRetVal[0] = this->mFoo;
		pRetVal[1] = this->mBar;
	}
	return 2;
}

ExitGames::Common::JString& SampleCustomType::toString(ExitGames::Common::JString& retStr, bool withTypes) const
{
	return retStr = ExitGames::Common::JString(L"<") + (withTypes?ExitGames::Common::JString(L"(")+EG_STR_UCHAR+L")":L"") + mFoo + L", " + (withTypes?ExitGames::Common::JString(L"(")+EG_STR_UCHAR+L")":L"") + mBar + L">";
}



SampleCustomType2::SampleCustomType2(void)
{
}

SampleCustomType2::SampleCustomType2(const ExitGames::Common::Dictionary<ExitGames::Common::JString, SampleCustomType>& dic)
{
	mDic = dic;
}

SampleCustomType2::SampleCustomType2(const SampleCustomType2& toCopy)
	: super()
{
	mDic = toCopy.mDic;
}

SampleCustomType2::~SampleCustomType2(void)
{
}

SampleCustomType2& SampleCustomType2::operator=(const SampleCustomType2& toCopy)
{
	mDic = toCopy.mDic;
	return *this;
}

bool SampleCustomType2::compare(const ExitGames::Common::CustomTypeBase& other) const
{
	return mDic == static_cast<const SampleCustomType2&>(other).mDic;
}

void SampleCustomType2::duplicate(ExitGames::Common::CustomTypeBase* pRetVal) const
{
	*reinterpret_cast<SampleCustomType2*>(pRetVal) = *this;
}

void SampleCustomType2::deserialize(const nByte* pData, short length)
{
	ExitGames::Common::DeSerializer d(pData, length);
	ExitGames::Common::Object o;
	d.pop(o);
	mDic = ExitGames::Common::ValueObject<ExitGames::Common::Dictionary<ExitGames::Common::JString, SampleCustomType> >(o).getDataCopy();
}

short SampleCustomType2::serialize(nByte* pRetVal) const
{
	ExitGames::Common::Serializer s;
	s.push(mDic);
	if(pRetVal)
		MEMCPY(pRetVal, s.getData(), s.getSize());
	return s.getSize();
}

ExitGames::Common::JString& SampleCustomType2::toString(ExitGames::Common::JString& retStr, bool withTypes) const
{
	return retStr = ExitGames::Common::JString(L"<") + (withTypes?mDic.typeToString():L"")+mDic.toString(withTypes) + L">";
}