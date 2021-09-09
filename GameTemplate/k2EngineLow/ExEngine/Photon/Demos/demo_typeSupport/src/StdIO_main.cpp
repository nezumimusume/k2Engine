#include "TypeSupportApplication.h"
#include "StdIO_UIListener.h"

int main(int argc, const char* argv[])
{
	TypeSupportApplication::run(new StdIO_UIListener());
	return 0;
}