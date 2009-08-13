#include "config.ih"

Config::Config()
{
	stringstream s;
	s << "localhost:" << optimization::Constants::CommandPort;

	commandUri = s.str();

	registerProperty("command uri", commandUri);
}
