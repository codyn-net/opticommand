#include "application.ih"

static int
periodic_readline()
{
	Glib::MainContext::get_default()->iteration(false);
	return 0;
}

bool Application::run(Glib::RefPtr<Glib::MainLoop> loop) 
{
	os::Signals::install();
	os::Signals::onInterrupt.addData(*this, &Application::onInterrupt, loop);
	
	stringstream s;
	s << optimization::Constants::CommandPort;
	
	string host = "localhost";
	string port = s.str();
	
	parseUri(host, port);
	
	d_main = loop;
	
	prompt() << Ansi::Blue << "* Connecting to master at " << Ansi::Green << host << ":" << port << Ansi::None << endl;
	
	/* Connect to master */
	d_client = Client::Tcp(host, port);
	
	if (!d_client)
	{
		prompt() << Ansi::Red << "* Could not connect to master" << Ansi::None << endl;
		return false;
	}
	else
	{
		prompt() << Ansi::Blue << "* Welcome, ready to receive orders" << Ansi::None << endl;
	}
	
	d_client.onData().add(*this, &Application::onData);
	d_client.onClosed().add(*this, &Application::onClosed);
	
	Timer timer;
	bool warned = false;
	
	string historyfile = Glib::get_user_config_dir() + "/.optcmd_history";
	read_history(historyfile.c_str());
	
	d_running = true;
	d_waitForResponse = false;
	rl_event_hook = periodic_readline;
	
	while (d_running)
	{
		if (!d_waitForResponse)
		{
			string pr = RL_PROMPT_START_IGNORE + Ansi::Yellow + RL_PROMPT_END_IGNORE + ">>> " + RL_PROMPT_START_IGNORE + Application::Ansi::None + RL_PROMPT_END_IGNORE;
			char *line_read = readline(pr.c_str());
		
			if (line_read && *line_read)
			{
				string data;
				bool ret = parseCommand(line_read, data);
			
				cout << (!ret ? (Ansi::Red + Ansi::Bold) : "") << data << Ansi::None << endl;
				add_history(line_read);
			}
		
			if (line_read)
			{
				free(line_read);
			}
			else
			{
				d_running = false;
				cout << endl;
			}
			
			timer.mark();
			warned = false;
		}
		else if (timer.since() > 5 && !warned)
		{
			cout << "* Waiting for response... (^C to break)" << flush;
			warned = true;
		}

		d_main->get_context()->iteration(false);
	}
	
	write_history(historyfile.c_str());
	d_client.onClosed().remove(*this, &Application::onClosed);

	return true;
}
