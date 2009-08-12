#include "application.ih"

void Application::showListJobs(optimization::messages::command::ListJobsResponse const &response)
{
	for (int i = 0; i < response.jobs_size(); ++i)
	{
		optimization::messages::command::Job const &job = response.jobs(i);
		
		if (i != 0)
		{
			cout << endl;
		}
		
		printJob(job);
	}
}
