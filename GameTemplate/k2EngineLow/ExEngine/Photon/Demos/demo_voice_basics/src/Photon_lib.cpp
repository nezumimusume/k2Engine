#include "LoadBalancing-cpp/inc/Enums/DirectMode.h"
#include "PhotonVoice-cpp/inc/AudioGen.h"
#include "PhotonVoice-cpp/inc/OpusCodec.h"
#include "PhotonVoice-cpp/inc/Transport/LoadBalancingTransport.h"
#include "AudioIO.h"
#include "DelayProcessor.h"
#include "Photon_lib.h"

using namespace ExitGames::Common;
using namespace ExitGames::Common::Helpers;
using namespace ExitGames::Common::MemoryManagement;
using namespace ExitGames::LoadBalancing;
using namespace ExitGames::Photon;
using namespace ExitGames::Voice;
using namespace ExitGames::Voice::AudioUtil;

static const bool DIRECT = false; // send audio peer to peer (debug echo mode is not supported)
static const bool CLIENT_ECHO = false; // for each remote voice, create local voice and broadcast incoming stream through it, can be useful for local testing when sending audio peer to peer - using this mode when other clients are inside the same room is not supported and leads to undefined behavior!
static const bool DEBUG_ECHO_MODE_INIT = false; // initial state of local voice debug echo mode (streaming back to client via server)
static const JString appID = L"<no-app-id>"; // set your app id here
static const JString appVersion = L"1.0";

static JString gameName = L"VoiceBasics";

static const JString PLAYER_NAME = L"user";
static const int MAX_SENDCOUNT = 100;

Echo::Echo(LocalVoiceAudio<float>* localVoice, ILogger* logger, const VoiceInfo& voiceInfo)
	: mpLocalVoice(localVoice)
	, mpLogger(logger)
	, mVoiceInfo(voiceInfo)
{
	mpLogger->logInfo(L"Creating Client Echo for " + mVoiceInfo.toString());
}

Echo::~Echo(void)
{
	mpLogger->logInfo(L"Destroying Client Echo for " + mVoiceInfo.toString());
	mpLocalVoice->removeSelf();
}

void Echo::frameDataCallback(const Buffer<float>& frame)
{
	mpLocalVoice->pushDataAsync(frame);
}

static void echoFrameDataCallback(void* opaque, const Buffer<float>& frame)
{
	static_cast<Echo*>(opaque)->frameDataCallback(frame);
}

static void echoRemoteVoiceRemoveCallback(void* opaque)
{
	delete(static_cast<Echo*>(opaque));
}

static void frameDataCallback(void* opaque, const Buffer<float>& frame)
{
	IAudioOut<float>* audioOut = static_cast<IAudioOut<float>*>(opaque);
	audioOut->push(frame);
}

RemoteVoiceData::RemoteVoiceData(ISyncAudioOut<float>* out, AudioStreamPlayer<float>* player, JVector<RemoteVoiceData*>* container)
	: out(out)
	, player(player)
	, container(container)
{
}

RemoteVoiceData::~RemoteVoiceData(void)
{
	delete out;
	delete player;
}

template <typename T>
void del(const T* e)
{
	delete e;
}

void PhotonLib::remoteVoiceRemoveCallback(void* opaque)
{
	RemoteVoiceData* d = static_cast<RemoteVoiceData*>(opaque);
	while(d->container->removeElement(d));
	delete d;
}

void PhotonLib::remoteVoiceInfoCallback(void* opaque, int channelId, int playerId, nByte voiceId, const VoiceInfo& voiceInfo, RemoteVoiceOptions& options)
{
	static_cast<PhotonLib*>(opaque)->remoteVoiceInfoCallback(channelId, playerId, voiceId, voiceInfo, options);
}

void PhotonLib::remoteVoiceInfoCallback(int channelId, int playerId, nByte voiceId, const VoiceInfo& voiceInfo, RemoteVoiceOptions& options)
{
	if(CLIENT_ECHO)
	{
		LocalVoiceAudio<float>* pV = mpVoiceClient->createLocalVoiceAudio<float>(voiceInfo, AudioDesc(voiceInfo.getSamplingRate(), voiceInfo.getChannels()));
		Echo* pE = new Echo(pV, mpTransport, voiceInfo);
		options.setOutput(pE, echoFrameDataCallback);
		options.setRemoteVoiceRemoveAction(pE, echoRemoteVoiceRemoveCallback);
		pV->getVoiceDetector()->setOn(false);
	}
	else
	{
		ISyncAudioOut<float>* pOut = audioIOCreateOut();
		AudioStreamPlayer<float>* pPlayer = new AudioStreamPlayer<float>(*mpTransport, *pOut, L"AudioStreamPlayer", true);

		pPlayer->start(voiceInfo.getSamplingRate(), voiceInfo.getChannels(), voiceInfo.getFrameDurationSamples(), 200);
		RemoteVoiceData* pRVD = new RemoteVoiceData(pOut, pPlayer, &mAudioPlayers);
		mAudioPlayers.addElement(pRVD);
		options.setRemoteVoiceRemoveAction(pRVD, remoteVoiceRemoveCallback);

		// either setOutput or setDecoder
		//options.setOutput(player, frameDataCallback);
		options.setDecoder(SharedPointer<IDecoder>(new Opus::Decoder<float>(pPlayer, frameDataCallback, *mpTransport), del<IDecoder>));
	}
}

PhotonLib::PhotonLib(UIListener* uiListener)
#ifdef _EG_MS_COMPILER
#	pragma warning(push)
#	pragma warning(disable:4355)
#endif
	: mLoadBalancingClient(*this, appID, appVersion, ConnectionProtocol::UDP, true, RegionSelectionMode::SELECT)
	, mState(State::INITIALIZED)
	, mpOutputListener(uiListener)
	, mSendCount(0)
	, mReceiveCount(0)
	, mVoicesCreated(false)
#ifdef _EG_MS_COMPILER
#	pragma warning(pop)
#endif
{
	mpTransport = new LoadBalancingTransport(mLoadBalancingClient, *this, DIRECT);
	mpVoiceClient = new VoiceClient(mpTransport);
	mpVoiceClient->setOnRemoteVoiceInfoAction(this, PhotonLib::remoteVoiceInfoCallback);

	mpTransport->setDebugOutputLevel(DEBUG_RELEASE(DebugLevel::INFO, DebugLevel::WARNINGS)); // all instances of VoiceClient that use mpTransport
	mLoadBalancingClient.setDebugOutputLevel(DEBUG_RELEASE(DebugLevel::INFO, DebugLevel::WARNINGS)); // that instance of LoadBalancingClient and its implementation details
	mLogger.setListener(*this);
	mLogger.setDebugOutputLevel(DEBUG_RELEASE(DebugLevel::INFO, DebugLevel::WARNINGS)); // this class
	Base::setListener(this);
	Base::setDebugOutputLevel(DEBUG_RELEASE(DebugLevel::INFO, DebugLevel::WARNINGS)); // all classes that inherit from Base

	initAudoIO();
}

PhotonLib::~PhotonLib(void)
{
	for(unsigned int i=0; i<mAudioSources.getSize(); ++i)
		mAudioSources[i]->release();
	mAudioSources.removeAllElements();
	for(unsigned int i=0; i<mLocalVoices.getSize(); ++i)
		mLocalVoices[i]->removeSelf();
	finitAudoIO();
	delete mpVoiceClient;
	delete mpTransport;
}

void PhotonLib::update(void)
{
	switch(mState)
	{
	case State::INITIALIZED:
		connect();
		mState = State::CONNECTING;
		break;
	case State::CONNECTED:
	{
		RoomOptions opt;
		if(DIRECT)
			opt.setDirectMode(DirectMode::ALL_TO_ALL);
		mLoadBalancingClient.opJoinOrCreateRoom(gameName, opt);
		mState = State::JOINING;
	}
		break;
	case State::JOINED:
		//sendData();
		break;
	case State::RECEIVED_DATA:
		mLoadBalancingClient.opLeaveRoom();
		mState = State::LEAVING;
		break;
	case State::LEFT:
		mLoadBalancingClient.disconnect();
		mState = State::DISCONNECTING;
		break;
	case State::DISCONNECTED:
		//mState = State::INITIALIZED;
		break;
	default:
		break;
	}
	mLoadBalancingClient.service();
	mpTransport->service();
	mpVoiceClient->service();
	for(unsigned int i = 0; i<mAudioPlayers.getSize(); i++)
		mAudioPlayers[i]->player->service();
}

void PhotonLib::onKeyPressed(int key)
{
	switch(key)
	{
	case 'c':
		connect();
		break;
	case 'd':
		disconnect();
		break;
	case 'e':
		for(unsigned int i=0; i<mLocalVoices.getSize(); ++i)
		{
			mLocalVoices[i]->setDebugEchoMode(!mLocalVoices[i]->getDebugEchoMode());
			mpOutputListener->writeString(mLocalVoices[i]->getName()+L" echo: "+(mLocalVoices[i]->getDebugEchoMode()?L"ON":L"OFF"));
		}
		break;
	}
}

void PhotonLib::connect(void)
{
	mLoadBalancingClient.connect(ExitGames::LoadBalancing::ConnectOptions().setAuthenticationValues(ExitGames::LoadBalancing::AuthenticationValues().setUserID(ExitGames::Common::JString()+GETTIMEMS())).setUsername(PLAYER_NAME+GETTIMEMS()));
}

void PhotonLib::disconnect(void)
{
	mLoadBalancingClient.disconnect();
}

JString PhotonLib::getStateString(void)
{
	switch(mState)
	{
	case State::INITIALIZED:
		return L"disconnected";
	case State::CONNECTING:
		return L"connecting";
	case State::CONNECTED:
		return L"connected";
	case State::JOINING:
		return L"joining";
	case State::JOINED:
		return L"";//JString(L"ingame\nsent event Nr. ")+mSendCount+L"\nreceived event Nr. "+mReceiveCount;
	case State::SENT_DATA:
		return JString(L"sending completed")+L"\nreceived event Nr. "+mReceiveCount;
	case State::RECEIVED_DATA:
		return L"receiving completed";
	case State::LEAVING:
		return L"leaving";
	case State::LEFT:
		return L"left";
	case State::DISCONNECTING:
		return L"disconnecting";
	case State::DISCONNECTED:
		return L"disconnected";
	default:
		return L"unknown state";
	}
}

void PhotonLib::sendData(void)
{
	Hashtable event;
	event.put(static_cast<nByte>(0), ++mSendCount);
	// send to ourselves only
	int myPlayerNumber = mLoadBalancingClient.getLocalPlayer().getNumber();
	mLoadBalancingClient.opRaiseEvent(true, event, 0, RaiseEventOptions().setTargetPlayers(&myPlayerNumber, 1));
	if(mSendCount>=MAX_SENDCOUNT)
		mState = State::SENT_DATA;
}

void PhotonLib::debugReturn(int /*debugLevel*/, const JString& string)
{
	mpOutputListener->writeString(string);
}

void PhotonLib::connectionErrorReturn(int errorCode)
{
	EGLOG(DebugLevel::ERRORS, L"code: %d", errorCode);
	mpOutputListener->writeString(JString(L"received connection error ")+errorCode);
	mState = State::DISCONNECTED;
}

void PhotonLib::clientErrorReturn(int errorCode)
{
	EGLOG(DebugLevel::ERRORS, L"code: %d", errorCode);
	mpOutputListener->writeString(JString(L"received error ")+errorCode+L" from client");
}

void PhotonLib::warningReturn(int warningCode)
{
	EGLOG(DebugLevel::WARNINGS, L"code: %d", warningCode);
	mpOutputListener->writeString(JString(L"received warning ")+warningCode+L" from client");
}

void PhotonLib::serverErrorReturn(int errorCode)
{
	EGLOG(DebugLevel::ERRORS, L"code: %d", errorCode);
	mpOutputListener->writeString(JString(L"received error ")+errorCode+" from server");
}

void PhotonLib::joinRoomEventAction(int playerNr, const JVector<int>& playernrs, const Player& player)
{
	LoadBalancingListener::joinRoomEventAction(playerNr, playernrs, player);

	EGLOG(DebugLevel::INFO, L"%ls joined the game", player.getName().cstr());
	mpOutputListener->writeString(L"");
	mpOutputListener->writeString(JString(L"player ")+playerNr+L" "+player.getName()+L" has joined the game");

	if(!mVoicesCreated)
	{
/*		ToneAudioPusher<float>* pAudioSource0 = allocate<ToneAudioPusher<float> >(1440, 22, 10000, 1);
		VoiceInfo i0=VoiceInfo::createAudioOpus(48000, pAudioSource0->getChannels(), 10000, 30000);
		LocalVoice* v0 = mpVoiceClient->createLocalVoiceAudioFromSource(i0, *pAudioSource0);
		v0->setDebugEchoMode(DEBUG_ECHO_MODE_INIT);
		mAudioSources.addElement(pAudioSource0);
		mLocalVoices.addElement(v0);

		ToneAudioPusher<float>* pAudioSource1 = allocate<ToneAudioPusher<float> >(440, 22, 10000, 2);
		VoiceInfo i1=VoiceInfo::createAudioOpus(24000, pAudioSource1->getChannels(), 60000, 30000);
		LocalVoice* v1 = mpVoiceClient->createLocalVoiceAudioFromSource(i1, *pAudioSource1);
		v1->setDebugEchoMode(DEBUG_ECHO_MODE_INIT);
		mAudioSources.addElement(pAudioSource1);
		mLocalVoices.addElement(v1);*/

		IAudioPusher<float>* pAudioSource2 = audioIOCreateMic();
		VoiceInfo i2 = VoiceInfo::createAudioOpus(16000, pAudioSource2->getChannels(), 20000, 30000);
		// default or user's decoder
		LocalVoiceAudio<float>* v2 = mpVoiceClient->createLocalVoiceAudioFromSource(i2, *pAudioSource2, 0, SharedPointer<IEncoder>(new Opus::Encoder<float>(i2, *mpTransport), del<IEncoder>));
		v2->setDebugEchoMode(DEBUG_ECHO_MODE_INIT);
		v2->getVoiceDetector()->setOn(false);

		// processor with allocation default or provided by user
		v2->addPostProcessor(SharedPointer<IProcessor<float>>(new DelayProcessor<float>(4000, 0.2f), del<IProcessor<float>>));
		mAudioSources.addElement(pAudioSource2);
		mLocalVoices.addElement(v2);

		mVoicesCreated = true;
	}
	EGLOG(DebugLevel::INFO, L"\n\n\n                         Press 'e' to toggle debug echo mode\n\n");
}

void PhotonLib::leaveRoomEventAction(int playerNr, bool isInactive)
{
	LoadBalancingListener::leaveRoomEventAction(playerNr, isInactive);

	EGLOG(DebugLevel::INFO, L"");
	mpOutputListener->writeString(L"");
	mpOutputListener->writeString(JString(L"player ")+playerNr+L" has left the game");
}

void PhotonLib::customEventAction(int playerNr, nByte eventCode, const Object& eventContentObj)
{
	LoadBalancingListener::customEventAction(playerNr, eventCode, eventContentObj);

	Hashtable eventContent = ValueObject<Hashtable>(eventContentObj).getDataCopy();
	switch(eventCode)
	{
	case 0:
		if(eventContent.getValue((nByte)0))
			mReceiveCount = ((ValueObject<int64>*)(eventContent.getValue((nByte)0)))->getDataCopy();
		if(mState==State::SENT_DATA && mReceiveCount>=mSendCount)
		{
			mState = State::RECEIVED_DATA;
			mSendCount = 0;
			mReceiveCount = 0;
		}
		break;
	default:
		break;
	}

}

void PhotonLib::connectReturn(int errorCode, const JString& errorString, const JString& region, const JString& cluster)
{
	EGLOG(DebugLevel::INFO, L"connected to cluster "+cluster+L" of region "+region);
	if(errorCode)
	{
		EGLOG(DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mState = State::DISCONNECTING;
		return;
	}
	mpOutputListener->writeString(L"connected to cluster "+cluster);
	mState = State::CONNECTED;
}

void PhotonLib::disconnectReturn(void)
{
	LoadBalancingListener::disconnectReturn();

	EGLOG(DebugLevel::INFO, L"");
	mpOutputListener->writeString(L"disconnected");
	mState = State::DISCONNECTED;
}

void PhotonLib::createRoomReturn(int localPlayerNr, const Hashtable& gameProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString)
{
	LoadBalancingListener::createRoomReturn(localPlayerNr, gameProperties, playerProperties, errorCode, errorString);

	EGLOG(DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mpOutputListener->writeString(L"opCreateRoom() failed: "+errorString);
		mState = State::CONNECTED;
		return;
	}

	EGLOG(DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	mpOutputListener->writeString(L"... room "+mLoadBalancingClient.getCurrentlyJoinedRoom().getName()+" has been created");
	mpOutputListener->writeString(L"regularly sending dummy events now");
	mState = State::JOINED;
}

void PhotonLib::joinOrCreateRoomReturn(int localPlayerNr, const Hashtable& gameProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString)
{
	LoadBalancingListener::joinOrCreateRoomReturn(localPlayerNr, gameProperties, playerProperties, errorCode, errorString);

	EGLOG(DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mpOutputListener->writeString(L"opJoinOrCreateRoom() failed: "+errorString);
		mState = State::CONNECTED;
		return;
	}

	EGLOG(DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	mpOutputListener->writeString(L"... room "+mLoadBalancingClient.getCurrentlyJoinedRoom().getName()+" has been entered");
	mpOutputListener->writeString(L"regularly sending dummy events now");
	mState = State::JOINED;
}

void PhotonLib::joinRandomOrCreateRoomReturn(int localPlayerNr, const Hashtable& gameProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString)
{
	LoadBalancingListener::joinRandomOrCreateRoomReturn(localPlayerNr, gameProperties, playerProperties, errorCode, errorString);

	EGLOG(DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mpOutputListener->writeString(L"opJoinRandomOrCreateRoom() failed: "+errorString);
		mState = State::CONNECTED;
		return;
	}

	EGLOG(DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	mpOutputListener->writeString(L"... room "+mLoadBalancingClient.getCurrentlyJoinedRoom().getName()+" has been entered");
	mpOutputListener->writeString(L"regularly sending dummy events now");
	mState = State::JOINED;
}

void PhotonLib::joinRoomReturn(int localPlayerNr, const Hashtable& gameProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString)
{
	LoadBalancingListener::joinRoomReturn(localPlayerNr, gameProperties, playerProperties, errorCode, errorString);

	EGLOG(DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mpOutputListener->writeString(L"opJoinRoom() failed: "+errorString);
		mState = State::CONNECTED;
		return;
	}
	EGLOG(DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	mpOutputListener->writeString(L"... room "+mLoadBalancingClient.getCurrentlyJoinedRoom().getName()+" has been successfully joined");
	mpOutputListener->writeString(L"regularly sending dummy events now");

	mState = State::JOINED;
}

void PhotonLib::joinRandomRoomReturn(int localPlayerNr, const Hashtable& gameProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString)
{
	LoadBalancingListener::joinRandomRoomReturn(localPlayerNr, gameProperties, playerProperties, errorCode, errorString);

	EGLOG(DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mpOutputListener->writeString(L"opJoinRandomRoom() failed: "+errorString);
		mState = State::CONNECTED;
		return;
	}

	EGLOG(DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	mpOutputListener->writeString(L"... room "+mLoadBalancingClient.getCurrentlyJoinedRoom().getName()+" has been successfully joined");
	mpOutputListener->writeString(L"regularly sending dummy events now");
	mState = State::JOINED;
}

void PhotonLib::leaveRoomReturn(int errorCode, const JString& errorString)
{
	LoadBalancingListener::leaveRoomReturn(errorCode, errorString);

	EGLOG(DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mpOutputListener->writeString(L"opLeaveRoom() failed: "+errorString);
		mState = State::DISCONNECTING;
		return;
	}
	mState = State::LEFT;
	mpOutputListener->writeString(L"room has been successfully left");
}

void PhotonLib::joinLobbyReturn(void)
{
	EGLOG(DebugLevel::INFO, L"");
	mpOutputListener->writeString(L"joined lobby");
}

void PhotonLib::leaveLobbyReturn(void)
{
	EGLOG(DebugLevel::INFO, L"");
	mpOutputListener->writeString(L"left lobby");
}

void PhotonLib::onAvailableRegions(const JVector<JString>& /*availableRegions*/, const JVector<JString>& /*availableRegionServers*/)
{
	mLoadBalancingClient.selectRegion(L"eu");
}