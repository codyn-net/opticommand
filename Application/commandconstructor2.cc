#include "application.ih"

Application::Command::Command(string const &command, Application::CommandHandler handler, int minargs, int maxargs) 
:
	command(command),
	handler(handler)
{
	argsRange[0] = minargs;
	argsRange[1] = maxargs;
}
