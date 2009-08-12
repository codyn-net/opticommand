#include "application.ih"

void Application::printJob(optimization::messages::command::Job const &job)
{
	time_t t = static_cast<time_t>(job.time());

	cout << "  [" << job.name() << "]" << endl
	     << "    * Optimizer:   " << job.optimizer() << endl
	     << "    * Progress:    " << job.iteration() << "/" << job.maxiterations() << endl
	     << "    * Last update: " << (t == 0 ? "" : ctime(&t)) << endl
	     << "    * Priority:    " << job.priority() << endl;
}
