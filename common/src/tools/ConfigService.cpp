#include <stdexcept>

#include "tools/ConfigService.h"
#include "tools/file/PropertyFileHandler.h"
#include "system/FileSystem.h"

namespace urchin
{

	ConfigService::ConfigService() :
			Singleton<ConfigService>(),
			isInitialized(false)
	{

	}

	ConfigService::~ConfigService()
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

		isInitialized=true;
	}
	
	int ConfigService::getIntValue(const std::string &propertyName) const
	{
		checkState();

		std::map<std::string, std::string>::const_iterator it = properties.find(propertyName);
		if(it!=properties.end())
		{
			return Converter::toInt(it->second);
		}

		throw std::invalid_argument("The property " + propertyName + " doesn't exist.");
	}
	
	unsigned int ConfigService::getUnsignedIntValue(const std::string &propertyName) const
	{
		checkState();

		std::map<std::string, std::string>::const_iterator it = properties.find(propertyName);
		if(it!=properties.end())
		{
			return Converter::toUnsignedInt(it->second);
		}

		throw std::invalid_argument("The property " + propertyName + " doesn't exist.");
	}

	float ConfigService::getFloatValue(const std::string &propertyName) const
	{
		checkState();

		std::map<std::string, std::string>::const_iterator it = properties.find(propertyName);
		if(it!=properties.end())
		{
			return Converter::toFloat(it->second);
		}

		throw std::invalid_argument("The property "+ propertyName + " doesn't exist.");
	}
	
	double ConfigService::getDoubleValue(const std::string &propertyName) const
	{
		checkState();

		std::map<std::string, std::string>::const_iterator it = properties.find(propertyName);
		if(it!=properties.end())
		{
			return Converter::toDouble(it->second);
		}

		throw std::invalid_argument("The property " + propertyName + "doesn't exist.");
	}

	std::string ConfigService::getStringValue(const std::string &propertyName) const
	{
		checkState();

		std::map<std::string, std::string>::const_iterator it = properties.find(propertyName);
		if(it!=properties.end())
		{
			return it->second;
		}

		throw std::invalid_argument("The property " + propertyName + " doesn't exist.");
	}
	
	char ConfigService::getCharValue(const std::string &propertyName) const
	{
		checkState();

		std::map<std::string, std::string>::const_iterator it = properties.find(propertyName);
		if(it!=properties.end())
		{
			return (it->second)[0];
		}

		throw std::invalid_argument("The property " + propertyName + " doesn't exist.");
	}

	bool ConfigService::getBoolValue(const std::string &propertyName) const
	{
		checkState();

		std::map<std::string, std::string>::const_iterator it = properties.find(propertyName);
		if(it!=properties.end())
		{
			bool value = false;
			if(it->second.compare("true")==0)
			{
				value = true;
			}
			return value;
		}

		throw std::invalid_argument("The property "+ propertyName + " doesn't exist.");
	}

}
