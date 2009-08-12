#include "application.ih"

void Application::showJobInfo(optimization::messages::command::JobInfoResponse const &response)
{
	printJob(response.job());
}
