#pragma once

class UIListener;

class TypeSupportApplication
{
public:
	//Launch main loop
	//this function takes uwnership of listener
	static void run(UIListener* listener);
};