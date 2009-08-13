#include "application.ih"

bool Application::handleQuit(vector<string> const &args, string &data)
{
	d_running = false;
	return true;
}
