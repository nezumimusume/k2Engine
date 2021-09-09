#include "BasicsApplication.h"
#include "StdIO_UIListener.h"

int main(void)
{
	StdIO_UIListener* l = new StdIO_UIListener();
	BasicsApplication::run(l);
	delete l;
}