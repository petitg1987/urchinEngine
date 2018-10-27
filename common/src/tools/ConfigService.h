#ifndef URCHINENGINE_CONFIGSERVICE_H
#define URCHINENGINE_CONFIGSERVICE_H

#include <string>
#include <map>

#include "pattern/singleton/Singleton.h"

namespace urchin
{

	/**
	* Service to handle configuration properties files
	*/
	class ConfigService : public Singleton<ConfigService>
	{
		public:
			friend class Singleton<ConfigService>;

			bool isInitialized() const;

			void loadProperties(const std::string &);
			void loadProperties(const std::string &, const std::string &);

			unsigned getUnsignedIntValue(const std::string &) const;
			float getFloatValue(const std::string &) const;
			std::string getStringValue(const std::string &) const;
			char getCharValue(const std::string &) const;
			bool getBoolValue(const std::string &) const;
		
		private:
			ConfigService();
			~ConfigService() override = default;

			void checkState() const;

			std::map<std::string, std::string> properties;
			std::map<std::string, float> floatProperties;
			std::map<std::string, unsigned int> unsignedIntProperties;

			bool bIsInitialized;
	};
}

#endif
