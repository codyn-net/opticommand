#ifndef __COMMAND_APPLICATION_H__
#define __COMMAND_APPLICATION_H__

#include <network/network.hh>
#include <glibmm.h>
#include <vector>
#include <os/os.hh>
#include <optimization/Messages/messages.hh>
#include <iosfwd>

namespace command
{
	class Application
	{
		struct Commands
		{
			enum Values
			{
				AddJob = 0,
				RemoveJob,
				ListJobs,
				JobInfo,
				Help,
				NumCommands			
			};
		};
		
		typedef bool (Application::*CommandHandler)(std::vector<std::string> const &args, std::string &data);
		
		struct Command
		{
			std::string command;
			CommandHandler handler;
			
			int argsRange[2];
			
			Command();
			Command(std::string const &command, CommandHandler handler, int minargs, int maxargs);
		};
		
		network::Client d_client;
		std::vector<std::string> d_arguments;
		Glib::RefPtr<Glib::MainLoop> d_main;
		
		bool d_running;
		bool d_waitForResponse;
		Glib::ustring d_sendCommand;
		
		Command d_commands[Commands::NumCommands];

		public:
			struct Ansi
			{
				static std::string None;
				static std::string Bold;
				static std::string Blue;
				static std::string Red;
				static std::string Green;
				static std::string Yellow;
			};

			Application(int argc, char **argv);
			
			bool run(Glib::RefPtr<Glib::MainLoop> loop);
		private:
			bool onInterrupt(Glib::RefPtr<Glib::MainLoop> loop);
			
			void parseUri(std::string &host, std::string &port);
			
			void onData(os::FileDescriptor::DataArgs &args);
			void onClosed(int fd);
			
			void initCommands();
			bool parseCommand(std::string const &line, std::string &data);
			void parseArguments(int &argc, char **&argv);
			
			bool handleAddJob(std::vector<std::string> const &args, std::string &data);
			bool handleRemoveJob(std::vector<std::string> const &args, std::string &data);
			bool handleListJobs(std::vector<std::string> const &args, std::string &data);
			bool handleHelp(std::vector<std::string> const &args, std::string &data);
			bool handleJobInfo(std::vector<std::string> const &args, std::string &data);
			
			void showJobInfo(optimization::messages::command::JobInfoResponse const &response);
			void showListJobs(optimization::messages::command::ListJobsResponse const &response);
			
			void printJob(optimization::messages::command::Job const &job);
			
			std::ostream &prompt() const;
			bool sendCommand(optimization::messages::command::Command &command, std::string &data);
			
			bool interactiveMode(std::string const &host, std::string const &port);
			bool directMode(std::string const &host, std::string const &port);
	};
}

#endif /* __COMMAND_APPLICATION_H__ */
