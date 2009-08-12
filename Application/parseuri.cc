#include "application.ih"

void Application::parseUri(string &host, string &port) 
{
	if (d_arguments.size() <= 1)
		return;

	string::size_type stype;
	string uri = d_arguments[1];
	stype = uri.find(":");

	if (stype != string::npos)
	{
		host = uri.substr(0, stype - 1);
		port = uri.substr(stype + 1);
	}
	else
	{
		if (uri != "")
			host = uri;
		
		if (d_arguments.size() > 2 && d_arguments[2] != "")
			port = d_arguments[2];
	}
}
