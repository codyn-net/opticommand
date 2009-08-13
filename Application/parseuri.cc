#include "application.ih"

void Application::parseUri(string &host, string &port) 
{
	vector<string> parts = String(opticommand::Config::instance().commandUri).split(":", 2);
	host = parts[0];
	
	if (parts.size() == 1)
	{
		stringstream s;
		s << Constants::CommandPort;
		
		port = s.str();
	}
	else
	{
		port = parts[1];
	}
}
