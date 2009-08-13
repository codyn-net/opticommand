#include "application.ih"

Application::Application(int argc, char **argv) 
{
	uid_t uid = getuid();
	struct passwd *pw;
	
	pw = getpwuid(uid);
	
	if (pw)
	{
		d_user = pw->pw_name;
	}
	
	d_hasTerminal = isatty(0);
	
	if (!d_hasTerminal)
	{
		Ansi::None = "";
		Ansi::Bold = "";
		Ansi::Red = "";
		Ansi::Green = "";
		Ansi::Yellow = "";
		Ansi::Blue = "";
	}
	
	parseArguments(argc, argv);
	initCommands();
}
