#include "application.ih"

Application::Application(int argc, char **argv) 
{
	parseArguments(argc, argv);
	initCommands();
}
