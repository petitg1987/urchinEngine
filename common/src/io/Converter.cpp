#include <iostream>
#include <sstream>

#include "io/Converter.h"

namespace urchin
{

	Converter::Converter()
	{

	}

	Converter::~Converter()
	{

	}

	int Converter::toInt(std::string str)
	{
		std::istringstream iss(str);
		int value;
		iss >> value;
		return value;
	}

	unsigned int Converter::toUnsignedInt(std::string str)
	{
		std::istringstream iss(str);
		unsigned int value;
		iss >> value;
		return value;
	}

	float Converter::toFloat(std::string str)
	{
		std::locale::global(std::locale("C")); //for float

		std::istringstream iss(str);
		float value;
		iss >> value;
		return value;
	}

	double Converter::toDouble(std::string str)
	{
		std::istringstream iss(str);
		double value;
		iss >> value;
		return value;
	}

	char Converter::toChar(std::string str)
	{
		return str[0];
	}

}
