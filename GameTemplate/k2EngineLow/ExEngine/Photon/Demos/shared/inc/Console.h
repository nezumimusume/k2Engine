#include "OutputListener.h"

class ConsoleImplementation;

class Console : public OutputListener
{
public:
	Console(void);
	~Console(void);
	void update(void);
	void write(const ExitGames::Common::JString& str);
	void writeLine(const ExitGames::Common::JString& str=ExitGames::Common::JString());
	static Console& get(void);
private:
#if defined _EG_ANDROID_PLATFORM || defined _EG_APPLE_PLATFORM || defined _EG_PS4_PLATFORM
	ConsoleImplementation* mpImp;
#endif
};