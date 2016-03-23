#ifndef ENGINE_CONFIGSERVICE_H
#define ENGINE_CONFIGSERVICE_H

#include <string>
#include <map>

#include "pattern/singleton/Singleton.h"
#include "io/Converter.h"

namespace urchin
{

	/**
	* Service to handle configuration properties files
	*/
	class ConfigService : public Singleton<ConfigService>
	{
		public:
			friend class Singleton<ConfigService>;
		
			void loadProperties(const std::string &);
			void loadProperties(const std::string &, const std::string &);
		
			int getIntValue(const std::string &) const;
			unsigned getUnsignedIntValue(const std::string &) const;
			float getFloatValue(const std::string &) const;
			double getDoubleValue(const std::string &) const;
			std::string getStringValue(const std::string &) const;
			char getCharValue(const std::string &) const;
			bool getBoolValue(const std::string &) const;
		
		private:
			ConfigService();
			virtual ~ConfigService();

			void checkState() const;

			std::map<std::string, std::string> properties;
			bool isInitialized;
	};
}

#endif
