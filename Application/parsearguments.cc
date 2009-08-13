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
	
	Glib::OptionContext context;
	
	context.set_main_group(group);
	context.parse(argc, argv);
}
