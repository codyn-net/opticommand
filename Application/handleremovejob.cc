#include "application.ih"

bool Application::handleRemoveJob(vector<string> const &args, string &data) 
{
	optimization::messages::command::Command cmd;

	cmd.set_type(optimization::messages::command::RemoveJob);
	cmd.mutable_rmjob()->set_name(args[0]);

	return sendCommand(cmd, data);
}
