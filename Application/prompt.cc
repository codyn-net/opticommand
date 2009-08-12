#include "application.ih"

ostream &Application::prompt() const
{
	return (cout << Ansi::Yellow << ">>> " << Ansi::None);
}
