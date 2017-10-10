#include <sstream>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "tools/file/PropertyFileHandler.h"

namespace urchin
{

	PropertyFileHandler::PropertyFileHandler(const std::string &propertiesFilePath) :
		propertiesFilePath(propertiesFilePath)
	{

	}

	std::map<std::string, std::string> PropertyFileHandler::loadPropertyFile() const
	{
		std::map<std::string, std::string> properties;

		std::ifstream file;

		std::string propertyName;
		std::string equalSign;
		std::string propertyValue;

		file.open(propertiesFilePath, std::ios::in);
		if(file.fail())
		{
			throw std::invalid_argument("Cannot open the file " + propertiesFilePath + ".");
		}

		do
		{
			std::string buffer;
			nextLine(file, buffer);

			std::istringstream iss(buffer);
			iss >> propertyName;

			if(buffer.length()==0 || propertyName[0]=='#')
			{ //empty or commented line
				continue;
			}

			iss >> equalSign >> propertyValue;
			properties[propertyName] = propertyValue;
		}while(!file.eof());

		return properties;
	}

	void PropertyFileHandler::savePropertyFile(const std::map<std::string, std::string> &properties) const
	{
		std::ofstream file;
		file.open(propertiesFilePath, std::ios::out);
		if(file.fail())
		{
			throw std::invalid_argument("Cannot open the file " + propertiesFilePath + ".");
		}

		for (const auto &property : properties)
		{
			file << property.first << " = " << property.second <<"\n";
		}

		file.close();
	}

	void PropertyFileHandler::nextLine(std::ifstream &file, std::string &buffer) const
	{
		do
		{
			std::getline(file, buffer);

			//delete '\r'
			unsigned long length = buffer.length()-1;
			if(length >=0 && buffer[length]=='\r')
			{
				buffer.resize(length);
			}

		}while(buffer.length()==0 && !file.eof());
	}

}
