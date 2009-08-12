#include "application.ih"

bool Application::sendCommand(optimization::messages::command::Command &command, string &data) 
{
	string serialized;
	
	if (!Messages::create(command, serialized))
	{
		data = "Could not serialize command...";
		return false;
	}
	
	d_waitForResponse = true;
	d_client.write(serialized);

	return true;
}
