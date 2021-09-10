// demo_particle_console.cpp : Defines the entry point for the console application.
//

#include "Console.h"
#include "../../src/DemoConstants.h"
#include "../../src/LoadBalancingListener.h"
#include "ConsoleView.h"

using namespace ExitGames::LoadBalancing;
using namespace ExitGames::Common;
using namespace ExitGames::Photon;

int getcharIfKbhit(void);

#ifdef _EG_WINDOWS_PLATFORM
#include <conio.h>

int getcharIfKbhit(void)
{
	int res = _kbhit();
	if(res)
		res = _getch();
	return res;
}
#elif defined _EG_PS4_PLATFORM
#include "Controller.h"
#include "Keyboard.h"

int getcharIfKbhit(void)
{
	static Controller controller;
	static Keyboard* pKeyboard = NULL;
	wchar_t controllerInput = controller.readInput();
	wchar_t* keyboardInput = NULL;
	if(pKeyboard)
		keyboardInput = pKeyboard->readInput();
	if(controllerInput == '1')
		pKeyboard = MemoryManagement::allocate<Keyboard>();
	else if(controllerInput == '0')
		return 27;
	if(keyboardInput)
	{
		MemoryManagement::deallocate(pKeyboard);
		pKeyboard = NULL;
		return tolower(*keyboardInput);
	}
	return controllerInput;
}
#else
#include <termios.h>
#include <fcntl.h>

int getcharIfKbhit(void)
{
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	return ch;
}
#endif

bool gUseTcp = false;

void info(Client& lbc);
void trafficStats(Client& lbc);

static bool gSendAcksOnly = false;
static nByte gSendGroup;
static bool gUseGroups = false;
static JVector<nByte> gGroups;

static void usage()
{
	Console::get().writeLine(L"Photon Particle Demo");
	Console::get().writeLine();
#ifdef _EG_PS4_PLATFORM
	Console::get().writeLine(L"Cross - Open the on screen keyboard for command input");
	Console::get().writeLine(L"Circle - Close the application");
#else
	Console::get().writeLine(L"Esc - close the application");
#endif
	Console::get().writeLine();
	Console::get().writeLine(L"available commands:");
	Console::get().writeLine(L"h - print this help");
	Console::get().writeLine(L"d - disconnect");
	Console::get().writeLine(L"c - connect");
	Console::get().writeLine(L"/ - toggle \"send acks only\"");
	Console::get().writeLine(L"g - toggle interest groups");
	Console::get().writeLine(L"0-9 - if interest groups are activated, remove/add group for receiving, else set group for sending");
	Console::get().writeLine(L"i - show info");
	Console::get().writeLine(L"s - show traffic stats");
	Console::get().writeLine(L"r - reset traffic stats");
	Console::get().writeLine(L"t - toggle traffic stats");
	Console::get().writeLine(L"l - list all channels and messages");
	Console::get().writeLine();
}

int main(int argc, char* argv[])
{
	usage();
	LoadBalancingListener lbl(new ConsoleView());
	Client lbc(lbl, appID, appVersion, gUseTcp?ExitGames::Photon::ConnectionProtocol::TCP:ExitGames::Photon::ConnectionProtocol::UDP);
	lbc.setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS));
	ExitGames::Common::Base::setListener(&lbl);
	ExitGames::Common::Base::setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS));
//	lbc.setCRCEnabled(true);
	lbl.setLBC(&lbc);
	lbc.setTrafficStatsEnabled(true);
//	lbc.setLimitOfUnreliableCommands(15);
	Console::get().writeLine(L"connecting...");
	lbl.connect(JString(L"NVC")+GETTIMEMS());
	while(true)
	{
		if(gSendAcksOnly)
		{
			lbc.serviceBasic();
			while(lbc.dispatchIncomingCommands());
			lbc.sendAcksOnly();
		}
		else
		{
			//lbc.service();
			lbc.serviceBasic();
			while(lbc.dispatchIncomingCommands());
			while(lbc.sendOutgoingCommands());
		}
		lbl.service();

		int k = getcharIfKbhit();
		if(k == 27)
			break;
		if(k == 'h')
			usage();
		if(k == 'd')
		{
			Console::get().writeLine(L"disconnecting...");
			lbc.disconnect();
		}
		if(k == 'c')
		{
			Console::get().writeLine(L"connecting...");
			lbl.connect(JString(L"NVC")+GETTIMEMS());
		}
		if(k == '/')
		{
			Console::get().writeLine(gSendAcksOnly?L"disabling \"sendAcksOnly\"...":L"enabling \"sendAcksOnly\"...");
			gSendAcksOnly = !gSendAcksOnly;
		}
		if(k == 'g')
		{
			lbl.setUseGroups(false); // reset autogroups
			gUseGroups = !gUseGroups;
			JVector<nByte> empty;
			if(gUseGroups)
			{
				Console::get().writeLine(L"activating interest groups...");
				lbc.opChangeGroups(&empty, &gGroups);
			}
			else
			{
				Console::get().writeLine(L"deactivating interest groups...");
				lbc.opChangeGroups(&empty, NULL);
			}
		}
		if(k >= '0' && k <= '9')
		{
			lbl.setUseGroups(false); // reset autogroups
			nByte g = k - '0';
			if(gUseGroups)
			{
				JVector<nByte> v;
				if(gGroups.contains(g))
				{
					Console::get().writeLine(JString(L"removing interest group nr ") + g + L" from list of interest groups to receive updates for...");
					gGroups.removeElement(g);
					v.addElement(g);
					lbc.opChangeGroups(&v, NULL);
				}
				else
				{
					Console::get().writeLine(JString(L"adding interest group nr ") + g + L" to list of interest groups to receive updates for...");
					gGroups.addElement(g);
					v.addElement(g);
					lbc.opChangeGroups(NULL, &v);
				}
			}
			else
			{
				Console::get().writeLine(JString(L"setting interest group to send updates to group nr ") + g + L" ...");
				gSendGroup = g;
				lbl.setSendGroup(g);
			}
			info(lbc);
		}

		if(k == 'i')
		{
			Console::get().writeLine(L"showing info...");
			info(lbc);
		}
		if(k == 's')
		{
			Console::get().writeLine(L"showing traffic stats...");
			trafficStats(lbc);
		}
		if(k == 'r')
		{
			Console::get().writeLine(L"resetting traffic stats...");
			lbc.resetTrafficStats();
			trafficStats(lbc);
		}
		if(k == 't')
		{
			Console::get().writeLine(lbc.getTrafficStatsEnabled()?L"disabling traffic stats...":L"enabling traffic stats...");
			lbc.setTrafficStatsEnabled(!lbc.getTrafficStatsEnabled());
			trafficStats(lbc);
		}
		Console::get().update();
		int t = GETTIMEMS();
		while(GETTIMEMS() < t + 100);
	}
	return 0;
}

void info(Client& lbc)
{
	Console::get().writeLine(JString() + L"getState: " + lbc.getState());
	Console::get().writeLine(JString() + L"getDisconnectedCause: " + lbc.getDisconnectedCause());
	Console::get().writeLine(JString() + L"getTimestampOfLastSocketReceive: " + lbc.getTimestampOfLastSocketReceive());
	Console::get().writeLine(JString() + L"getPacketLossByCRC: " + lbc.getPacketLossByCRC());
	Console::get().writeLine(JString() + L"gUseGroups: " + gUseGroups);
	Console::get().writeLine(JString() + L"gSendGroup: " + gSendGroup);
	Console::get().writeLine(JString() + L"gGroups:");
	for(unsigned int i=0; i<gGroups.getSize(); ++i)
		Console::get().write(JString() + L" " + gGroups[i]);
	Console::get().writeLine();
}

void trafficStats(Client& lbc)
{
	Console::get().writeLine(JString() + L"TrafficStats: enabled ms" + lbc.getTrafficStatsElapsedMs() + L"ms");
	Console::get().writeLine(JString() + L"TrafficStatsGameLevel: " + lbc.getTrafficStatsGameLevel().toString());
	Console::get().writeLine(JString() + L"TrafficStatsGameLevel vital: " + lbc.getTrafficStatsGameLevel().toStringVitalStats());
	Console::get().writeLine(JString() + L"TrafficStats vital: " + lbc.vitalStatsToString(true));

	for(int i=0; i<2; ++i)
	{
		const TrafficStats* s;
		if(!i)
		{
			s = &lbc.getTrafficStatsOutgoing();
			Console::get().writeLine(JString() + L"TrafficStatsOutgoing: ========");
		}
		else
		{
			s = &lbc.getTrafficStatsIncoming();
			Console::get().writeLine(JString() + L"TrafficStatsIncoming: ========");
		}
		Console::get().writeLine(JString() + s->getPackageHeaderSize());
		Console::get().writeLine(JString() + s->getReliableCommandCount());
		Console::get().writeLine(JString() + s->getUnreliableCommandCount());
		Console::get().writeLine(JString() + s->getFragmentCommandCount());
		Console::get().writeLine(L"---");
		Console::get().writeLine(JString() + s->getControlCommandCount());
		Console::get().writeLine(JString() + s->getTotalPacketCount());
		Console::get().writeLine(JString() + s->getTotalCommandsInPackets());
		Console::get().writeLine(JString() + s->getReliableCommandBytes());
		Console::get().writeLine(L"---");
		Console::get().writeLine(JString() + s->getUnreliableCommandBytes());
		Console::get().writeLine(JString() + s->getFragmentCommandBytes());
		Console::get().writeLine(JString() + s->getControlCommandBytes());
		Console::get().writeLine(JString() + s->getTotalCommandCount());
		Console::get().writeLine(L"---");
		Console::get().writeLine(JString() + s->getTotalCommandBytes());
		Console::get().writeLine(JString() + s->getTotalPacketBytes());
		Console::get().writeLine(JString() + s->getTimestampOfLastAck());
		Console::get().writeLine(JString() +  s->getTimestampOfLastReliableCommand());
		Console::get().writeLine(L"========");
	}
}