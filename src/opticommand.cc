#include "application.hh"

#include <glibmm.h>

using namespace opticommand;

int main(int argc, char *argv[])
{
	Glib::init();
	Glib::RefPtr<Glib::MainLoop> loop = Glib::MainLoop::create();
	
	Application application(argc, argv);
	
	return application.Run(loop) ? 0 : 1;
}
