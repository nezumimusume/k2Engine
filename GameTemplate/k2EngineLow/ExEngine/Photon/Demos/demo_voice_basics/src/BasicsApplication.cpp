#include "Common-cpp/inc/Common.h"
#include "BasicsApplication.h"
#include "Photon_lib.h"
#include "UIListener.h"

using namespace ExitGames::Common;

void BasicsApplication::run(UIListener* listener)
{
	PhotonLib lib(listener);

	int key;
	while((key=listener->getKeyPressed()) != 27/*ESC*/)
	{
		lib.onKeyPressed(key);
		static JString lastStateString;
		lib.update();
		if(lib.getStateString().length() && lib.getStateString() != lastStateString)
			listener->writeString(lastStateString=lib.getStateString());
		SLEEP(20);
	}
	listener->onLibClosed();
}