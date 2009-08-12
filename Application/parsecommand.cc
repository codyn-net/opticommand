#include "application.ih"

bool Application::parseCommand(string const &line, string &data) 
{
	vector<string> parts = String(line).split(" ");
	
	string cmd = parts[0];
	parts.erase(parts.begin());
	
	int numArgs = parts.size();
	
	for (size_t i = 0; i < Commands::NumCommands; ++i)
	{
		Command &c = d_commands[i];
		
		if (!c.handler || c.command != cmd)
			continue;
		
		if (numArgs < c.argsRange[0] || (numArgs > c.argsRange[1] && c.argsRange[1] != -1))
		{
			data = "Invalid number of arguments";
			return false;
		}
		else
		{
			return (this->*(c.handler))(parts, data);
		}
	}
 	
	data = string("Command not found (") + cmd + ")";
	return false;
}
