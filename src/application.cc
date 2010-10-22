#include "application.hh"

#include <algorithm>
#include <optimization/messages.hh>
#include <optimization/constants.hh>
#include <jessevdk/os/os.hh>
#include <fstream>
#include <jessevdk/base/base.hh>

#include <readline/readline.h>
#include <readline/history.h>
#include <gcrypt.h>

#include <termios.h>
#include <unistd.h>
#include <iomanip>

#include <cmath>

#include "config.hh"

using namespace std;
using namespace jessevdk::network;
using namespace opticommand;
using namespace optimization::messages;
using namespace optimization;
using namespace jessevdk::base;
using namespace jessevdk::os;

string Application::Ansi::None = "\e[0m";
string Application::Ansi::Bold = "\e[1m";
string Application::Ansi::Red = "\e[31m";
string Application::Ansi::Green = "\e[32m";
string Application::Ansi::Yellow = "\e[33m";
string Application::Ansi::Blue = "\e[34m";

Application *Application::s_instance = 0;

Application &
Application::Instance()
{
	return *s_instance;
}

Application &
Application::Initialize(int argc, char **argv)
{
	if (!s_instance)
	{
		s_instance = new Application(argc, argv);
	}

	return *s_instance;
}

Application::Application(int argc, char **argv)
{
	d_hasTerminal = isatty(0);

	gcry_check_version(NULL);

	ParseArguments(argc, argv);

	if (!d_hasTerminal || Config::Instance().DisableColors)
	{
		Ansi::None = "";
		Ansi::Bold = "";
		Ansi::Red = "";
		Ansi::Green = "";
		Ansi::Yellow = "";
		Ansi::Blue = "";
	}

	InitCommands();
}

Application::Command::Command()
:
	Name(""),
	Handler(0)
{
}


Application::Command::Command(string const                &name,
                              Application::CommandHandler  handler,
                              int                          minargs,
                              int                          maxargs)
:
	Name(name),
	Handler(handler)
{
	ArgsRange[0] = minargs;
	ArgsRange[1] = maxargs;
}


bool
Application::DirectMode(string const &host,
                        string const &port)

{
	d_client = Client::Tcp(host, port);

	if (!d_client)
	{
		cerr << "Could not connect to master" << endl;
		return false;
	}

	d_client.OnData().Add(*this, &Application::OnData);
	d_client.OnClosed().Add(*this, &Application::OnClosed);

	vector<string> cmds = String(d_sendCommand).Split(";");

	for (vector<string>::iterator iter = cmds.begin(); iter != cmds.end(); ++iter)
	{
		string data;
		bool ret = ParseCommand(String(*iter).Strip(), data);

		if (!ret)
		{
			cerr << data << endl;
			return false;
		}

		while (d_waitForResponse)
		{
			d_main->get_context()->iteration(false);
		}

		if (!d_lastResult)
		{
			return false;
		}
	}

	d_client.OnClosed().Remove(*this, &Application::OnClosed);
	return true;
}

bool
Application::HandleHelp(vector<string> const &args,
                        string               &data)
{
	stringstream s;

	s << endl;

	for (size_t i = 0; i < Commands::NumCommands; ++i)
	{
		s << d_commands[i].Name << endl;
	}

	data = s.str();
	return true;
}

bool
Application::HandleProgress(vector<string> const &args,
                            string               &data)
{
	command::Command cmd;
	stringstream idstream(args[0]);
	size_t id;

	idstream >> id;

	cmd.set_type(command::Progress);
	cmd.mutable_progress()->set_id(id);

	return SendCommand(cmd, data);
}

bool
Application::HandleIdle(vector<string> const &args,
                        string               &data)
{
	command::Command cmd;

	cmd.set_type(command::Idle);
	cmd.mutable_idle();

	return SendCommand(cmd, data);
}

bool
Application::HandleInfo(vector<string> const &args,
                        string               &data)
{
	command::Command cmd;
	stringstream idstream(args[0]);
	size_t id;

	idstream >> id;

	cmd.set_type(command::Info);
	cmd.mutable_info()->set_id(id);

	return SendCommand(cmd, data);
}

bool
Application::HandleList(vector<string> const &args,
                        string               &data)
{
	command::Command cmd;

	cmd.set_type(command::List);
	cmd.mutable_list();

	return SendCommand(cmd, data);
}

bool
Application::HandleQuit(vector<string> const &args,
                        string               &data)
{
	d_running = false;
	return true;
}

bool
Application::HandleKill(vector<string> const &args,
                        string               &data)
{
	command::Command cmd;
	stringstream idstream(args[0]);
	size_t id;

	idstream >> id;

	cmd.set_type(command::Kill);
	cmd.mutable_kill()->set_id(id);

	return SendCommand(cmd, data);
}

bool
Application::HandleSetPriority(vector<string> const &args,
                               string               &data)
{
	command::Command cmd;
	stringstream idstream(args[0]);
	size_t id;
	double priority;

	idstream >> id;

	cmd.set_type(command::SetPriority);
	cmd.mutable_setpriority()->set_id(id);

	idstream.clear();
	idstream.str(args[1]);

	idstream >> priority;

	cmd.mutable_setpriority()->set_priority(priority);

	return SendCommand(cmd, data);
}

string
Application::AskToken()
{
	struct termios t;

	tcgetattr(0, &t);
	t.c_lflag &= ~ECHO;
	tcsetattr(0, 0, &t);

	string line;

	cout << "Password: " << flush;

	getline(cin, line);

	t.c_lflag |= ECHO;
	tcsetattr(0, 0, &t);

	cout << endl;

	return line;
}

bool
Application::HandleAuthenticate(vector<string> const &args,
                                string               &data)
{
	command::Command cmd;

	cmd.set_type(command::Authenticate);
	cmd.mutable_authenticate()->set_token("");

	d_waitForResponse = true;

	return SendCommand(cmd, data);
}

void Application::InitCommands()
{
	d_commands[Commands::List] = Command("list", &Application::HandleList, 0, 0);
	d_commands[Commands::Info] = Command("info", &Application::HandleInfo, 1, 1);
	d_commands[Commands::Progress] = Command("progress", &Application::HandleProgress, 1, 1);
	d_commands[Commands::Idle] = Command("idle", &Application::HandleIdle, 0, 0);

	d_commands[Commands::Kill] = Command("kill", &Application::HandleKill, 1, 1);
	d_commands[Commands::SetPriority] = Command("set-priority", &Application::HandleSetPriority, 2, 2);
	d_commands[Commands::Authenticate] = Command("authenticate", &Application::HandleAuthenticate, 0, 0);

	d_commands[Commands::Quit] = Command("quit", &Application::HandleQuit, 0, 0);
	d_commands[Commands::Exit] = Command("exit", &Application::HandleQuit, 0, 0);

	d_commands[Commands::Help] = Command("help", &Application::HandleHelp, 0, 1);
}

char **
Application::CompleteCommand(string const &text)
{
	char **matches = NULL;

	vector<char *> cmds;

	for (size_t i = 0; i < Commands::NumCommands; ++i)
	{
		Command &c = d_commands[i];

		if (c.Handler && String(c.Name).StartsWith(text))
		{
			cmds.push_back(strdup(c.Name.c_str()));
		}
	}

	matches = (char **)malloc(sizeof(char *) * (cmds.size() + 1));

	for (size_t i = 0; i < cmds.size(); ++i)
	{
		matches[i] = cmds[i];
	}

	matches[cmds.size()] = NULL;
	return matches;
}

static int
periodic_readline()
{
	Glib::MainContext::get_default()->iteration(false);
	return 0;
}

static char **
try_command_completion (char const *text, int start, int end)
{
	char **matches = NULL;

	if (start == 0)
	{
		matches = Application::Instance().CompleteCommand(text);
	}

	return matches;
}

bool
Application::InteractiveMode(string const &host,
                             string const &port)
{
	Signals::Install();
	Signals::OnInterrupt.AddData(*this, &Application::OnInterrupt, d_main);

	if (d_hasTerminal)
	{
		Prompt() << Ansi::Blue << "* Connecting to master at " << Ansi::Green << host << ":" << port << Ansi::None << endl;
	}

	/* Connect to master */
	d_client = Client::Tcp(host, port);

	if (!d_client)
	{
		Prompt() << Ansi::Red << "* Could not connect to master" << Ansi::None << endl;
		return false;
	}
	else if (d_hasTerminal)
	{
		Prompt() << Ansi::Blue << "* Welcome, ready to receive orders" << Ansi::None << endl;
	}

	d_client.OnData().Add(*this, &Application::OnData);
	d_client.OnClosed().Add(*this, &Application::OnClosed);

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
	rl_attempted_completion_function = try_command_completion;

	while (d_running)
	{
		if (!d_waitForResponse)
		{
			string line;

			if (d_hasTerminal)
			{
				string pr = RL_PROMPT_START_IGNORE + Ansi::Yellow + RL_PROMPT_END_IGNORE +
				            ">>> " + RL_PROMPT_START_IGNORE + Application::Ansi::None + RL_PROMPT_END_IGNORE;

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
				bool ret = ParseCommand(line, data);

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

			timer.Mark();
			warned = false;
		}
		else if (timer.Since() > 5 && !warned)
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

	d_client.OnClosed().Remove(*this, &Application::OnClosed);

	return d_hasTerminal ? true : d_lastResult;
}

void
Application::OnClosed(int fd)
{
	if (d_sendCommand == "")
	{
		rl_replace_line("", 0);
		rl_redisplay();

		cout << endl;

		Prompt() << Ansi::Red << "* Connection with master closed" << Ansi::None;
	}

	d_running = false;
	d_waitForResponse = false;

	rl_done = 1;
}

void
Application::OnData(FileDescriptor::DataArgs &args)
{
	vector<command::Response> response;
	vector<command::Response>::iterator iter;

	Messages::Extract(args, response);

	for (iter = response.begin(); iter != response.end(); ++iter)
	{
		command::Response &r = *iter;

		if (r.status())
		{
			switch (r.type())
			{
				case command::Info:
					if (r.has_info())
					{
						ShowInfo(r.info());
					}
				break;
				case command::List:
					if (r.has_list())
					{
						ShowList(r.list());
					}
				break;
				case command::Authenticate:
					if (r.has_authenticate())
					{
						Authenticate(r.authenticate());
					}
				break;
				case command::Progress:
					if (r.has_progress())
					{
						ShowProgress(r.progress());
					}
				break;
				case command::Idle:
					if (r.has_idle())
					{
						ShowIdle(r.idle());
					}
				break;
				default:
					if (r.message() != "")
					{
						cout << r.message() << endl;
					}
				break;
			}

			d_lastResult = true;
		}
		else
		{
			cerr << Ansi::Red << Ansi::Bold << r.message() << Ansi::None << endl;
			d_lastResult = false;
		}

		d_waitForResponse = false;
	}
}

bool
Application::OnInterrupt(Glib::RefPtr<Glib::MainLoop> loop)
{
	if (!rl_line_buffer || *rl_line_buffer == '\0')
	{
		d_running = false;
		rl_done = 1;
	}
	else
	{
		rl_replace_line("", 0);
		rl_redisplay();

		d_waitForResponse = false;
	}

	return true;
}

void
Application::ParseArguments(int    &argc,
                            char **&argv)
{
	// Parse config file first
	opticommand::Config &config = opticommand::Config::Initialize(CONFDIR "/opticommand.conf");

	Glib::OptionGroup group("Command", "Optimization Commander");

	Glib::OptionEntry command;
	command.set_long_name("master");
	command.set_short_name('m');
	command.set_description("The master hostname[:port] to connect to");

	group.add_entry(command, config.MasterAddress);

	Glib::OptionEntry send;
	send.set_long_name("send");
	send.set_short_name('s');
	send.set_description("Send single command");

	group.add_entry(send, d_sendCommand);

	Glib::OptionEntry nocolors;
	nocolors.set_long_name("no-colors");
	nocolors.set_short_name('n');
	nocolors.set_description("Disable colors");

	group.add_entry(nocolors, config.DisableColors);

	Glib::OptionEntry raw;
	raw.set_long_name("raw");
	raw.set_short_name('r');
	raw.set_description("Raw");

	group.add_entry(raw, config.Raw);

	Glib::OptionContext context;

	context.set_main_group(group);
	context.parse(argc, argv);
}

bool
Application::ParseCommand(string const &line,
                          string       &data)
{
	vector<string> parts = String(line).Split(" ");

	string cmd = parts[0];
	parts.erase(parts.begin());

	int numArgs = parts.size();

	for (size_t i = 0; i < Commands::NumCommands; ++i)
	{
		Command &c = d_commands[i];

		if (!c.Handler || c.Name != cmd)
		{
			continue;
		}

		if (numArgs < c.ArgsRange[0] || (numArgs > c.ArgsRange[1] && c.ArgsRange[1] != -1))
		{
			data = "Invalid number of arguments";
			return false;
		}
		else
		{
			return (this->*(c.Handler))(parts, data);
		}
	}

	data = string("Command not found (") + cmd + ")";
	return false;
}

void
Application::ParseUri(string &host,
                      string &port)
{
	vector<string> parts = String(opticommand::Config::Instance().MasterAddress).Split(":", 2);
	host = parts[0];

	if (parts.size() == 1)
	{
		stringstream s;
		s << Constants::CommandPort;

		port = s.str();
	}
	else
	{
		port = parts[1];
	}
}

string
Application::FormatDate(time_t t) const
{
	char buffer[1024];
	size_t size;

	if (t == 0)
	{
		return "";
	}

	size = strftime(buffer, sizeof(buffer) - 1, "%A %d %B, %H:%M:%S", localtime(&t));
	buffer[size] = '\0';

	return buffer;
}

void
Application::PrintJob(command::Job const &job)
{
	time_t started = static_cast<time_t>(job.started());
	time_t updated = static_cast<time_t>(job.lastupdate());

	double pgs = job.progress() * 100;
	string bullet = "  * " + Ansi::Red;

	bool raw = Config::Instance().Raw;

	if (!raw)
	{
		cout << "[" << job.id() << "] " << Ansi::Blue << job.name() << Ansi::None << " (" << Ansi::Green << job.user() << Ansi::None << ")" << endl
		     << bullet << "Priority:     " << Ansi::None << job.priority() << endl << endl
		     << bullet << "Started:      " << Ansi::None << FormatDate(started) << endl
		     << bullet << "Last update:  " << Ansi::None << FormatDate(updated) << endl
		     << bullet << "Progress:     " << Ansi::None << fixed << setprecision(2) << pgs << " %" << endl << endl
		     << bullet << "Runtime:      " << Ansi::None << job.runtime() << endl
		     << bullet << "Tasks:        " << Ansi::None << Ansi::Green << job.taskssuccess() << Ansi::None << "/" << Ansi::Red << job.tasksfailed() << Ansi::None << " (" << Ansi::Green << "success" << Ansi::None << "/" << Ansi::Red << "failed" << Ansi::None << ")" << endl;
	}
	else
	{
		cout << job.id() << "\t" << job.name() << "\t" << job.user() << endl
		     << "priority\t" << job.priority() << endl
		     << "started\t" << started << endl
		     << "lastupdate\t" << updated << endl
		     << "progress\t" << job.progress() << endl
		     << "runtime\t" << job.runtime() << endl
		     << "taskssuccess\t" << job.taskssuccess() << endl
		     << "tasksfailed\t" << job.tasksfailed() << endl;
	}
}

ostream &
Application::Prompt() const
{
	if (d_hasTerminal)
	{
		return (cout << Ansi::Yellow << ">>> " << Ansi::None);
	}
	else
	{
		return cout;
	}
}

bool
Application::Run(Glib::RefPtr<Glib::MainLoop> loop)
{
	string host;
	string port;

	ParseUri(host, port);
	d_main = loop;

	if (d_sendCommand != "")
	{
		return DirectMode(host, port);
	}
	else
	{
		return InteractiveMode(host, port);
	}
}

bool
Application::SendCommand(command::Command &command,
                         string           &data)
{
	string serialized;

	if (!optimization::Messages::Create(command, serialized))
	{
		data = "Could not serialize command...";
		return false;
	}

	d_waitForResponse = true;
	d_client.Write(serialized);

	return true;
}

void
Application::ShowProgressRaw(command::ProgressResponse const &response)
{
	for (int i = 0; i < response.fitnesses_size(); ++i)
	{
		task::Identify::Fitness const &fit = response.fitnesses(i);

		if (i != 0)
		{
			cout << "\t";
		}

		cout << fit.name() << "\t" << fit.type();
	}

	cout << endl;

	for (int i = 0; i < response.items_size(); ++i)
	{
		task::Progress const &pgs = response.items(i);

		cout << pgs.tick();

		for (int j = 0; j < pgs.terms_size(); ++j)
		{
			task::Progress::Term const &term = pgs.terms(j);
			cout << "\t" << term.best() << "\t" << term.mean();
		}

		cout << endl;
	}
}

void
Application::ShowProgressArt(command::ProgressResponse const &response)
{
	if (response.items_size() == 0)
	{
		return;
	}

	size_t nrows = 15;
	size_t ncols = 70;

	double maxval = 0;
	double minval = 0;

	vector<vector<size_t> > marks;
	marks.resize(nrows);

	size_t maxtick = response.items(response.items_size() - 1).tick();

	if (maxtick < ncols)
	{
		ncols = maxtick;
	}

	for (int i = 0; i < response.items_size(); ++i)
	{
		task::Progress const &pgs = response.items(i);
		double val = pgs.terms(0).best();

		if (i == 0 || val > maxval)
		{
			maxval = val;
		}

		if (i == 0 || val < minval)
		{
			minval = val;
		}
	}

	double meanval = 0;
	size_t prevcol = 0;
	size_t numval = 0;

	for (int i = 0; i < response.items_size(); ++i)
	{
		task::Progress const &pgs = response.items(i);
		double val = pgs.terms(0).best();

		size_t col = (size_t)round(pgs.tick() / (double)maxtick * (ncols - 1));

		if (i == 0 || col == prevcol)
		{
			meanval += val;
			++numval;
		}
		else
		{
			meanval /= numval;

			size_t row = nrows - (size_t)round((meanval - minval) / (maxval - minval) * (nrows - 1)) - 1;
			marks[row].push_back(col);

			numval = 1;
			meanval = val;
		}

		prevcol = col;
	}

	for (size_t i = 0; i < marks.size(); ++i)
	{
		// Output row in fact
		vector<size_t> const &row = marks[i];
		size_t prev = -1;

		cout << "|";

		for (size_t j = 0; j < row.size(); ++j)
		{
			size_t num = row[j];

			cout << string(num - prev - 1, ' ') << '*';
			prev = num;
		}

		cout << endl;
	}

	cout << "+" << string(ncols, '-') << endl;
}

void
Application::ShowProgress(command::ProgressResponse const &response)
{
	if (Config::Instance().Raw)
	{
		ShowProgressRaw(response);
	}
	else
	{
		ShowProgressArt(response);
	}
}

void
Application::ShowInfo(command::InfoResponse const &response)
{
	PrintJob(response.job());
}

void
Application::ShowList(command::ListResponse const &response)
{
	bool raw = Config::Instance().Raw;

	for (int i = 0; i < response.jobs_size(); ++i)
	{
		optimization::messages::command::Job const &job = response.jobs(i);

		if (!raw)
		{
			cout << "[" << job.id() << "] "
			     << Ansi::Blue << job.name() << Ansi::None
			     << " (" << Ansi::Green << job.user() << Ansi::None
			     << ") " << setprecision(2) << (job.progress() * 100) << " %" << endl;
		}
		else
		{
			cout << job.id() << "\t" << job.name() << "\t" << job.user() << "\t" << job.progress() << endl;
		}
	}
}

void
Application::Authenticate(command::AuthenticateResponse const &response)
{
	gcry_md_hd_t hd;

	if (gcry_md_open(&hd, GCRY_MD_SHA1, 0) != GPG_ERR_NO_ERROR)
	{
		cerr << Ansi::Red << Ansi::Bold << "Could not create crypted" << Ansi::None << endl;
	}

	string passwd = AskToken();
	string challenge = response.challenge();

	gcry_md_write(hd, passwd.c_str(), strlen(passwd.c_str()));
	gcry_md_write(hd, challenge.c_str(), strlen(challenge.c_str()));

	uint8_t *ptr = gcry_md_read(hd, GCRY_MD_SHA1);
	stringstream token;

	for (size_t i = 0; i < gcry_md_get_algo_dlen(GCRY_MD_SHA1); ++i)
	{
		token << hex << setfill('0') << setw(2) << (unsigned short)ptr[i];
	}

	gcry_md_close(hd);

	string data;
	command::Command cmd;

	cmd.set_type(command::Authenticate);
	cmd.mutable_authenticate()->set_token(token.str());

	SendCommand(cmd, data);
}
