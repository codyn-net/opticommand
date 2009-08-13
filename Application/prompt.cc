#include "application.ih"

ostream &Application::prompt() const
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
