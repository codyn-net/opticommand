#include "application.ih"

bool Application::handleHelp(vector<string> const &args, string &data) 
{
	stringstream s;
	
	s << endl;

	for (size_t i = 0; i < Commands::NumCommands; ++i)
	{
		s << d_commands[i].command << endl;
	}

	data = s.str();
	return true;
}
