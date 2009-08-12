#include "application.ih"

Application::Application(int argc, char const **argv) 
{
	d_arguments.resize(argc);

	for (int i = 0; i < argc; ++i)
		d_arguments[i] = argv[i];
	
	initCommands();
}
