#ifndef URCHINENGINE_PROPERTYFILEHANDLER_H
#define URCHINENGINE_PROPERTYFILEHANDLER_H

#include <string>
#include <map>

namespace urchin
{

	class PropertyFileHandler
	{
		public:
			explicit PropertyFileHandler(const std::string &);

			std::map<std::string, std::string> loadPropertyFile() const;
			void savePropertyFile(const std::map<std::string, std::string> &) const;

		private:
			void nextLine(std::ifstream &file, std::string &buffer) const;

			std::string propertiesFilePath;
	};

}

#endif
