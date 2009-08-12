#include "Application/application.hh"
#include <base/base.hh>
#include <glibmm.h>

using namespace command;

int main(int argc, char const *argv[])
{
	Glib::init();
	Glib::RefPtr<Glib::MainLoop> loop = Glib::MainLoop::create();
	
	//base::Debug::enable(base::Debug::Domain::Network);

	Application application(argc, argv);
	
	return application.run(loop) ? 0 : 1;
}
