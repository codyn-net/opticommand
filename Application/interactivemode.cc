#include "application.ih"

static int
periodic_readline()
{
	Glib::MainContext::get_default()->iteration(false);
	return 0;
}

bool Application::interactiveMode(string const &host, string const &port)
{
	os::Signals::install();
	os::Signals::onInterrupt.addData(*this, &Application::onInterrupt, d_main);

	if (d_hasTerminal)
	{
		prompt() << Ansi::Blue << "* Connecting to master at " << Ansi::Green << host << ":" << port << Ansi::None << endl;
	}
	
	/* Connect to master */
	d_client = Client::Tcp(host, port);
	
	if (!d_client)
	{
		prompt() << Ansi::Red << "* Could not connect to master" << Ansi::None << endl;
		return false;
	}
	else if (d_hasTerminal)
	{
		prompt() << Ansi::Blue << "* Welcome, ready to receive orders" << Ansi::None << endl;
	}
	
	d_client.onData().add(*this, &Application::onData);
	d_client.onClosed().add(*this, &Application::onClosed);
	
	Timer timer;
	bool warned = false;
	d_lastResult = true;
	
	string historyfile = Glib::get_user_config_dir() + "/.optcmd_history";
	
	if (d_hasTerminal)
	{
		read_history(historyfile.c_str());
	}
	
	d_running = true;
	d_waitForResponse = false;
	rl_event_hook = periodic_readline;
	
	while (d_running)
	{
		if (!d_waitForResponse)
		{
			string line;
			
			if (d_hasTerminal)
			{
				string pr = RL_PROMPT_START_IGNORE + Ansi::Yellow + RL_PROMPT_END_IGNORE + ">>> " + (d_chain != "" ? "[" + d_chain + "] " : "") + RL_PROMPT_START_IGNORE + Application::Ansi::None + RL_PROMPT_END_IGNORE;
				char *line_read = readline(pr.c_str());
				
				if (line_read)
				{
					line = line_read;
					free(line_read);
				}
				else
				{
					d_running = false;
					cout << endl;
				}
			}
			else
			{
				if (!getline(cin, line))
				{
					d_running = false;
				}
			} 
		
			if (line != "")
			{
				string data;
				bool ret = parseCommand(line, data);
			
				if (data != "")
				{
					cout << (!ret ? (Ansi::Red + Ansi::Bold) : "") << data << Ansi::None << endl;
				}
				
				d_lastResult = ret;

				if (d_hasTerminal)
				{
					add_history(line.c_str());
				}
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
	
	if (d_hasTerminal)
	{
		write_history(historyfile.c_str());
	}

	d_client.onClosed().remove(*this, &Application::onClosed);
	
	return d_hasTerminal ? true : d_lastResult;
}
