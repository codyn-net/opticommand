#include "application.ih"

void Application::parseArguments(int &argc, char **&argv) 
{
	// Parse config file first
	opticommand::Config &config = opticommand::Config::initialize(CONFDIR "/opticommand.conf");

	Glib::OptionGroup group("Command", "Optimization Commander");

	Glib::OptionEntry command;
	command.set_long_name("command");
	command.set_short_name('c');
	command.set_description("Command uri");
	
	group.add_entry(command, config.commandUri);
	
	Glib::OptionEntry send;
	send.set_long_name("send");
	send.set_short_name('s');
	send.set_description("Send single command");
	
	group.add_entry(send, d_sendCommand);

	Glib::OptionEntry userent;
	userent.set_long_name("user");
	userent.set_short_name('u');
	userent.set_description("Used to determine where to save data (send mode)");
	
	group.add_entry(userent, d_user);
	
	Glib::OptionEntry chain;
	chain.set_long_name("chain");
	chain.set_short_name('c');
	chain.set_description("Chain job after previously added job (send mode)");
	
	group.add_entry(chain, d_chain);
	
	Glib::OptionContext context;
	
	context.set_main_group(group);
	context.parse(argc, argv);
}
