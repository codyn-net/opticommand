#include "application.ih"

bool Application::handleAddJob(vector<string> const &args, string &data) 
{
	xmlpp::DomParser parser;
	parser.set_substitute_entities();

	Glib::RefPtr<Gio::File> file = Gio::File::create_for_commandline_arg(args[0]);

	try
	{
		parser.parse_file(file->get_path());
	}
	catch (xmlpp::exception e)
	{
		data = string("Failed to parse job description: ") + e.what();
		return false;
	}

	// Make sure to process include stuff
	xmlpp::Document *doc = parser.get_document();
	
	if (xmlXIncludeProcess(doc->cobj()) < 0)
	{
		data = "Failed to parse includes";
		return false;
	}

	Glib::ustring xml = doc->write_to_string();
	optimization::messages::command::Command cmd;

	cmd.set_type(optimization::messages::command::AddJob);
	cmd.mutable_addjob()->set_description(xml);
	
	if (args.size() > 1)
	{
		stringstream s(args[1]);
		size_t priority;
		
		if (s >> priority)
			cmd.mutable_addjob()->set_priority(priority);
	}
	
	if (args.size() > 2)
	{
		cmd.mutable_addjob()->set_token(args[2]);
	}

	return sendCommand(cmd, data);
}
