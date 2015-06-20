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

	std::string Converter::toString(int value)
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}


	std::string Converter::toString(unsigned int value)
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}

	std::string Converter::toString(float value)
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}

	std::string Converter::toString(double value)
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}

	std::string Converter::toString(char value)
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
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
