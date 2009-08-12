#include "application.ih"

void Application::onClosed(int fd) 
{
	rl_replace_line("", 0);
	rl_redisplay();

	cout << endl;

	prompt() << Ansi::Red << "* Connection with master closed" << Ansi::None;

	d_running = false;
	d_waitForResponse = false;
	
	rl_done = 1;
}
