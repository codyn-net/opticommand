#include "application.ih"

bool Application::handleJobInfo(vector<string> const &args, string &data)
{
	optimization::messages::command::Command cmd;

	cmd.set_type(optimization::messages::command::JobInfo);
	cmd.mutable_jobinfo()->set_name(args[0]);

	return sendCommand(cmd, data);
}
