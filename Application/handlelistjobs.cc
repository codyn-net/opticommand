#include "application.ih"

bool Application::handleListJobs(vector<string> const &args, string &data) 
{
	optimization::messages::command::Command cmd;

	cmd.set_type(optimization::messages::command::ListJobs);

	return sendCommand(cmd, data);
}
