#include <iostream>
#include <limits>
#include <cmath>

#include "io/Converter.h"
#include "io/StringUtil.h"
#include "tools/logger/Logger.h"

namespace urchin
{

    bool Converter::isInt(const std::string &str)
    {
        std::istringstream iss(str);
        int value;
        iss >> std::noskipws >> value;
        return iss.eof() && !iss.fail();
    }

	int Converter::toInt(const std::string &str)
	{
		std::istringstream iss(str);
		int value;
		iss >> value;
		return value;
	}

    bool Converter::isUnsignedInt(const std::string &str)
    {
        std::istringstream iss(str);
        unsigned int value;
        iss >> std::noskipws >> value;
        return iss.eof() && !iss.fail();
    }

	unsigned int Converter::toUnsignedInt(const std::string &str)
	{
		std::istringstream iss(str);
		unsigned int value;
		iss >> value;
		return value;
	}

    bool Converter::isFloat(const std::string &str)
    {
        std::locale::global(std::locale("C")); //for float

        std::istringstream iss(str);
        float value;
        iss >> std::noskipws >> value;
        return iss.eof() && !iss.fail();
    }

	float Converter::toFloat(const std::string &str)
	{
		std::locale::global(std::locale("C")); //for float

		std::istringstream iss(str);
		float value;
		iss >> value;
		return value;
	}

    bool Converter::isChar(const std::string &str)
    {
        return str.size()==1;
    }

	char Converter::toChar(const std::string &str)
	{
		return str[0];
	}

	long long Converter::toLongLong(float value, float scale)
	{
		const float minValue = static_cast<float>(std::numeric_limits<long long>::min()) / scale;
		const float maxValue = static_cast<float>(std::numeric_limits<long long>::max()) / scale;

		if(value < 0.0)
		{
			if(value < minValue)
			{
				Logger::logger().log(Logger::ERROR, "Impossible to convert float " + std::to_string(value) + " in long long type.");
			}
			return std::llround(value * scale - 0.5);
		}
		else
		{
			if(value > maxValue)
			{
				Logger::logger().log(Logger::ERROR, "Impossible to convert float " + std::to_string(value) + " in long long type.");
			}
			return std::llround(value * scale + 0.5);
		}
	}

	float Converter::toFloat(long long value, float scale)
	{
		return static_cast<float>(value) / scale;
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
		for (const auto &stringValue : stringValues)
		{
			floatValues.push_back(toFloat(stringValue));
		}

		return floatValues;
	}

}
