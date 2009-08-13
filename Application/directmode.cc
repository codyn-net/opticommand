#include "application.ih"

bool Application::directMode(string const &host, string const &port)
{
	d_client = Client::Tcp(host, port);
	
	if (!d_client)
	{
		cerr << "Could not connect to master" << endl;
		return false;
	}
	
	d_client.onData().add(*this, &Application::onData);
	d_client.onClosed().add(*this, &Application::onClosed);
	
	string data;
	bool ret = parseCommand(d_sendCommand, data);
	
	if (!ret)
	{
		cerr << data << endl;
		return false;
	}
	else
	{
		cout << data << endl;
	}
	
	d_client.onClosed().remove(*this, &Application::onClosed);
	return true;
}
