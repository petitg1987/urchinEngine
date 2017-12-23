#include <stdexcept>

#include "tools/ConfigService.h"
#include "tools/file/PropertyFileHandler.h"
#include "io/Converter.h"
#include "system/FileSystem.h"

namespace urchin
{

	ConfigService::ConfigService() :
			Singleton<ConfigService>(),
			isInitialized(false)
	{

	}

	void ConfigService::checkState() const
	{
		if(!isInitialized)
		{
			throw std::runtime_error("Impossible to get this property: the configuration service hasn't been initialized.");
		}
	}

	void ConfigService::loadProperties(const std::string &propertiesFile)
	{
		loadProperties(propertiesFile, FileSystem::instance()->getResourcesDirectory());
	}

	/**
	 * @param workingDirectory Override the default working directory
	 */
	void ConfigService::loadProperties(const std::string &propertiesFile, const std::string &workingDirectory)
	{
		std::string propertiesFilePath = workingDirectory + propertiesFile;
		PropertyFileHandler propertyFileHandler(propertiesFilePath);
		
		properties = propertyFileHandler.loadPropertyFile();
        for(const auto &property : properties)
        { //build specific maps for performance reason (numeric conversion is slow)
            if(Converter::isUnsignedInt(property.second))
            {
                unsignedIntProperties[property.first] = Converter::toUnsignedInt(property.second);
            }
            if(Converter::isFloat(property.second))
            {
                floatProperties[property.first] = Converter::toFloat(property.second);
            }
        }

		isInitialized=true;
	}

	unsigned int ConfigService::getUnsignedIntValue(const std::string &propertyName) const
	{
		checkState();

		auto it = unsignedIntProperties.find(propertyName);
		if(it!=unsignedIntProperties.end())
		{
			return it->second;
		}

		throw std::invalid_argument("The property " + propertyName + " doesn't exist.");
	}

	float ConfigService::getFloatValue(const std::string &propertyName) const
	{
		checkState();

		auto it = floatProperties.find(propertyName);
		if(it!=floatProperties.end())
		{
			return it->second;
		}

		throw std::invalid_argument("The property "+ propertyName + " doesn't exist.");
	}

	std::string ConfigService::getStringValue(const std::string &propertyName) const
	{
		checkState();

		auto it = properties.find(propertyName);
		if(it!=properties.end())
		{
			return it->second;
		}

		throw std::invalid_argument("The property " + propertyName + " doesn't exist.");
	}
	
	char ConfigService::getCharValue(const std::string &propertyName) const
	{
		checkState();

		auto it = properties.find(propertyName);
		if(it!=properties.end())
		{
			return Converter::toChar(it->second);
		}

		throw std::invalid_argument("The property " + propertyName + " doesn't exist.");
	}

	bool ConfigService::getBoolValue(const std::string &propertyName) const
	{
		checkState();

		auto it = properties.find(propertyName);
		if(it!=properties.end())
		{
			bool value = false;
			if(it->second == "true")
			{
				value = true;
			}
			return value;
		}

		throw std::invalid_argument("The property "+ propertyName + " doesn't exist.");
	}

}
