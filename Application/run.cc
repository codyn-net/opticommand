#include "application.ih"

bool Application::run(Glib::RefPtr<Glib::MainLoop> loop) 
{
	string host;
	string port;
	
	parseUri(host, port);
	d_main = loop;

	if (d_sendCommand != "")
	{
		return directMode(host, port);
	}
	else
	{
		return interactiveMode(host, port);
	}
}
