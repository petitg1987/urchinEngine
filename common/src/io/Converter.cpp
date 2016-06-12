#include <iostream>
#include <sstream>

#include "io/Converter.h"
#include "io/StringUtil.h"

namespace urchin
{

	Converter::Converter()
	{

	}

	Converter::~Converter()
	{

	}

	int Converter::toInt(const std::string &str)
	{
		std::istringstream iss(str);
		int value;
		iss >> value;
		return value;
	}

	unsigned int Converter::toUnsignedInt(const std::string &str)
	{
		std::istringstream iss(str);
		unsigned int value;
		iss >> value;
		return value;
	}

	float Converter::toFloat(const std::string &str)
	{
		std::locale::global(std::locale("C")); //for float

		std::istringstream iss(str);
		float value;
		iss >> value;
		return value;
	}

	double Converter::toDouble(const std::string &str)
	{
		std::istringstream iss(str);
		double value;
		iss >> value;
		return value;
	}

	char Converter::toChar(const std::string &str)
	{
		return str[0];
	}

	Point2<float> Converter::toPoint2(const std::string &str)
	{
		std::vector<float> floatValues = floatSplit(str, 2);
		return Point2<float>(floatValues[0], floatValues[1]);
	}

	Point3<float> Converter::toPoint3(const std::string &str)
	{
		std::vector<float> floatValues = floatSplit(str, 3);
		return Point3<float>(floatValues[0], floatValues[1], floatValues[2]);
	}

	Point4<float> Converter::toPoint4(const std::string &str)
	{
		std::vector<float> floatValues = floatSplit(str, 4);
		return Point4<float>(floatValues[0], floatValues[1], floatValues[2], floatValues[3]);
	}

	Vector2<float> Converter::toVector2(const std::string &str)
	{
		std::vector<float> floatValues = floatSplit(str, 2);
		return Vector2<float>(floatValues[0], floatValues[1]);
	}

	Vector3<float> Converter::toVector3(const std::string &str)
	{
		std::vector<float> floatValues = floatSplit(str, 3);
		return Vector3<float>(floatValues[0], floatValues[1], floatValues[2]);
	}

	Vector4<float> Converter::toVector4(const std::string &str)
	{
		std::vector<float> floatValues = floatSplit(str, 4);
		return Vector4<float>(floatValues[0], floatValues[1], floatValues[2], floatValues[3]);
	}

	/**
	 * Split a string into float
	 * @param str String to split
	 * @param expectedSplit Number of expected split
	 * @return Split string in float
	 */
	std::vector<float> Converter::floatSplit(const std::string &str, unsigned int expectedSplit)
	{
		std::vector<std::string> stringValues;
		stringValues.reserve(expectedSplit);
		StringUtil::split(str, FLOAT_DELIMITOR, stringValues);

		if(stringValues.size()!=expectedSplit)
		{
			throw std::invalid_argument("Number of float expected: " + std::to_string(expectedSplit) + ", found: "
					+ std::to_string((unsigned int)stringValues.size()) + ". String value: " + str + ".");
		}

		std::vector<float> floatValues;
		floatValues.reserve(expectedSplit);
		for(unsigned int i=0; i<stringValues.size(); ++i)
		{
			floatValues.push_back(toFloat(stringValues[i]));
		}

		return floatValues;
	}

}
