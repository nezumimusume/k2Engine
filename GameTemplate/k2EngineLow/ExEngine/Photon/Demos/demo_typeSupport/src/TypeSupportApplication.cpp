#include "TypeSupportApplication.h"
#include "Photon_lib.h"
#include "UIListener.h"

UIListenerPtr gUIListener;

void TypeSupportApplication::run(UIListener* listener)
{
	gUIListener.reset(listener);

	CPhotonLib photonLib;
	photonLib.run();
	gUIListener->onLibClosed();
	gUIListener.reset();
}