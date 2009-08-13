#include "application.ih"

bool Application::handleChain(vector<string> const &args, string &data)
{
	if (args.size() == 0)
	{
		d_chain = "";
	}
	else
	{
		d_chain = args[0];
	}
	
	return true;
}
