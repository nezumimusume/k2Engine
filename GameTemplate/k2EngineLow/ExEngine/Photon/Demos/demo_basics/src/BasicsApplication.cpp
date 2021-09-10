#include "Common-cpp/inc/Common.h"
#include "BasicsApplication.h"
#include "Photon_lib.h"
#include "UIListener.h"

using namespace ExitGames::Common;

void BasicsApplication::run(UIListener* listener)
{
	PhotonLib lib(listener);
	
	while(!listener->anyKeyPressed())
	{
		static JString lastStateString;
		lib.update();
		if(lib.getStateString().length() && lib.getStateString() != lastStateString)
			listener->writeString(lastStateString=lib.getStateString());
		SLEEP(100);
	}
	listener->onLibClosed();
}