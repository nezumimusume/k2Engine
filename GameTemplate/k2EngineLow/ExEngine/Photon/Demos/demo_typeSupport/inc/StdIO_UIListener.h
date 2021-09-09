#include "UIListener.h"

class StdIO_UIListener: public UIListener
{
private:
	virtual void writeString(const ExitGames::Common::JString& str);
	virtual bool anyKeyPressed(void) const;
	virtual void onLibClosed(void);
};