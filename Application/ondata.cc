#include "application.ih"

void Application::onData(os::FileDescriptor::DataArgs &args) 
{
	vector<optimization::messages::command::Response> response;
	vector<optimization::messages::command::Response>::iterator iter;
	
	Messages::extract(args, response);

	for (iter = response.begin(); iter != response.end(); ++iter)
	{
		optimization::messages::command::Response &r = *iter;
		
		if (r.status())
		{
			switch (r.type())
			{
				case optimization::messages::command::JobInfo:
					showJobInfo(r.jobinfo());
				break;
				case optimization::messages::command::ListJobs:
					showListJobs(r.listjobs());
				break;
				default:
					if (r.message() != "")
					{
						cout << r.message() << endl;
					}
				break;
			}
			
			if (r.type() == optimization::messages::command::AddJob)
			{
				d_chain = "";
			}
			
			d_lastResult = true;
		}
		else
		{
			cout << Ansi::Red << Ansi::Bold << r.message() << Ansi::None << endl;
			d_lastResult = false;
		}
		
		d_waitForResponse = false;
	}
}
