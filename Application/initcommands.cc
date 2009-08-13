#include "application.ih"

void Application::initCommands() 
{
	d_commands[Commands::AddJob] = Command("add", &Application::handleAddJob, 1, 3);
	d_commands[Commands::RemoveJob] = Command("rm", &Application::handleRemoveJob, 1, 1);
	d_commands[Commands::ListJobs] = Command("list", &Application::handleListJobs, 0, 0);
	d_commands[Commands::JobInfo] = Command("info", &Application::handleJobInfo, 1, 1);
	d_commands[Commands::Chain] = Command("chain", &Application::handleChain, 0, 1);
	
	d_commands[Commands::Help] = Command("help", &Application::handleHelp, 0, 1);
}
