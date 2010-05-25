#include "config.hh"

#include <optimization/constants.hh>

using namespace std;
using namespace opticommand;

Config *Config::s_instance = 0;

Config::Config()
{
	stringstream s;
	s << "localhost:" << optimization::Constants::CommandPort;

	MasterAddress = s.str();

	DisableColors = false;
	Raw = false;

	Register("master address", MasterAddress);
	Register("disable colors", DisableColors);
	Register("raw", Raw);
}

Config &
Config::Initialize(string const &filename)
{
	if (!s_instance)
	{
		s_instance = new Config();
	}
	
	s_instance->Read(filename);
	return *s_instance;
}

Config &
Config::Instance()
{
	return *s_instance;
}
