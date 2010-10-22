#ifndef __OPTICOMMAND_APPLICATION_H__
#define __OPTICOMMAND_APPLICATION_H__

#include <jessevdk/network/network.hh>
#include <glibmm.h>
#include <vector>
#include <jessevdk/os/os.hh>
#include <optimization/messages.hh>
#include <iosfwd>

namespace opticommand
{
	class Application
	{
		struct Commands
		{
			enum Values
			{
				List = 0,
				Info,
				Kill,
				SetPriority,
				Authenticate,
				Progress,
				Idle,
				Quit,
				Exit,
				Help,
				NumCommands
			};
		};

		typedef bool (Application::*CommandHandler)(std::vector<std::string> const &args, std::string &data);

		struct Command
		{
			std::string Name;
			CommandHandler Handler;

			int ArgsRange[2];

			Command();
			Command(std::string const &name, CommandHandler handler, int minargs, int maxargs);
		};

		jessevdk::network::Client d_client;
		std::vector<std::string> d_arguments;
		Glib::RefPtr<Glib::MainLoop> d_main;

		bool d_running;
		bool d_waitForResponse;
		bool d_hasTerminal;
		bool d_lastResult;

		Glib::ustring d_authenticateToken;

		Glib::ustring d_sendCommand;

		Command d_commands[Commands::NumCommands];

		static Application *s_instance;

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

			static Application &Initialize(int argc, char **argv);
			static Application &Instance();

			bool Run(Glib::RefPtr<Glib::MainLoop> loop);

			char **CompleteCommand(std::string const &text);
		private:
			Application(int argc, char **argv);

			std::string AskToken();
			std::string FormatDate(time_t t) const;

			bool OnInterrupt(Glib::RefPtr<Glib::MainLoop> loop);

			void ParseUri(std::string &host, std::string &port);

			void OnData(jessevdk::os::FileDescriptor::DataArgs &args);
			void OnClosed(int fd);

			void InitCommands();
			bool ParseCommand(std::string const &line, std::string &data);
			void ParseArguments(int &argc, char **&argv);

			bool HandleList(std::vector<std::string> const &args, std::string &data);
			bool HandleInfo(std::vector<std::string> const &args, std::string &data);
			bool HandleKill(std::vector<std::string> const &args, std::string &data);
			bool HandleSetPriority(std::vector<std::string> const &args, std::string &data);
			bool HandleAuthenticate(std::vector<std::string> const &args, std::string &data);
			bool HandleHelp(std::vector<std::string> const &args, std::string &data);
			bool HandleProgress(std::vector<std::string> const &args, std::string &data);
			bool HandleIdle(std::vector<std::string> const &args, std::string &data);

			bool HandleQuit(std::vector<std::string> const &args, std::string &data);

			void ShowInfo(optimization::messages::command::InfoResponse const &response);
			void ShowList(optimization::messages::command::ListResponse const &response);
			void Authenticate(optimization::messages::command::AuthenticateResponse const &response);
			void ShowProgress(optimization::messages::command::ProgressResponse const &response);
			void ShowProgressRaw(optimization::messages::command::ProgressResponse const &response);
			void ShowProgressArt(optimization::messages::command::ProgressResponse const &response);
			void ShowIdle(optimization::messages::command::IdleResponse const &response);

			void PrintJob(optimization::messages::command::Job const &job);

			std::ostream &Prompt() const;
			bool SendCommand(optimization::messages::command::Command &command, std::string &data);

			bool InteractiveMode(std::string const &host, std::string const &port);
			bool DirectMode(std::string const &host, std::string const &port);
	};
}

#endif /* __OPTICOMMAND_APPLICATION_H__ */
