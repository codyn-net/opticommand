#include "application.ih"

bool Application::onInterrupt(Glib::RefPtr<Glib::MainLoop> loop) 
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
