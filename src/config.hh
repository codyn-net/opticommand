#ifndef __OPTICOMMAND_CONFIG_H__
#define __OPTICOMMAND_CONFIG_H__

#include <jessevdk/base/config.hh>
#include <glibmm.h>

namespace opticommand
{
	class Config : public jessevdk::base::Config
	{
		static Config *s_instance;

		public:
			Glib::ustring CommandUri;
			bool DisableColors;
			bool Raw;

			/* Constructor/destructor */
			static Config &Initialize(std::string const &filename);
			static Config &Instance();

			/* Public functions */
		private:
			/* Private functions */
			Config();
	};
}

#endif /* __OPTICOMMAND_CONFIG_H__ */
