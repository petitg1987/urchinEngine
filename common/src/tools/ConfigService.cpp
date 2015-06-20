#include <sstream>
#include <iostream>
#include <stdexcept>
#include <string.h>

#include "tools/ConfigService.h"
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

	void ConfigService::nextLine(std::ifstream &file, std::string &buffer)
	{
		do
		{
			std::getline(file, buffer);

			//delete '\r'
			int length = buffer.length()-1;
			if(length >=0 && buffer[length]=='\r')
			{
				buffer.resize(length);
			}

		}while(buffer.length()==0 && !file.eof());
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
		loadProperties(propertiesFile, FileSystem::instance()->getWorkingDirectory());
	}

	/**
	 * @param workingDirectory Override the default working directory
	 */
	void ConfigService::loadProperties(const std::string &propertiesFile, const std::string &workingDirectory)
	{
		std::ifstream file;
		std::istringstream iss;

		std::string propertyName;
		std::string equalSign;
		std::string propertyValue;
		
		std::string propertiesFilePath = workingDirectory + propertiesFile;
		file.open(propertiesFilePath.c_str(), std::ios::in);
		if(file.fail())
		{
			throw std::invalid_argument("Cannot open the file " + propertiesFilePath + ".");
		}
		
		do
		{
			std::string buffer;
			nextLine(file, buffer);
			
			iss.clear(); iss.str(buffer);
			iss >> propertyName;
			
			if(buffer.length()==0 || propertyName[0]=='#')
			{ //empty or commented line
				continue;
			}
			
			iss >> equalSign >> propertyValue;
			properties[propertyName] = propertyValue;
		}while(!file.eof());

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
