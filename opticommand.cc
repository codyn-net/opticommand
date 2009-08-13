#include "Application/application.hh"
#include <base/base.hh>
#include <glibmm.h>

using namespace command;

int main(int argc, char *argv[])
{
	Glib::init();
	Glib::RefPtr<Glib::MainLoop> loop = Glib::MainLoop::create();
	
	Application application(argc, argv);
	
	return application.run(loop) ? 0 : 1;
}
