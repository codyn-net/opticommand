#ifndef __OPTICOMMAND_CONFIG_H__
#define __OPTICOMMAND_CONFIG_H__

#include <base/Config/config.hh>
#include <glibmm.h>

namespace opticommand
{
	class Config : public base::Config
	{
		static Config *s_instance;

		public:
			Glib::ustring commandUri;
			
			/* Constructor/destructor */
			static Config &initialize(std::string const &filename);
			static Config &instance();
		
			/* Public functions */
		private:
			/* Private functions */
			Config();
	};
}

#endif /* __OPTICOMMAND_CONFIG_H__ */
