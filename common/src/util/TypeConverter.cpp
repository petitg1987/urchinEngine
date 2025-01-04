#include <iostream>
#include <limits>
#include <cmath>
#include <cassert>
#include <iomanip>

#include <util/TypeConverter.h>
#include <util/StringUtil.h>
#include <logger/Logger.h>

namespace urchin {

    bool TypeConverter::isInt(const std::string& str) {
        std::istringstream iss(str);
        int value = 0;
        iss >> std::noskipws >> value;
        return iss.eof() && !iss.fail();
    }

    int TypeConverter::toInt(const std::string& str) {
        #ifdef URCHIN_DEBUG
            assert(isInt(str));
        #endif
        std::istringstream iss(str);
        int value = 0;
        iss >> value;
        return value;
    }

    bool TypeConverter::isLong(const std::string& str) {
        std::istringstream iss(str);
        long value = 0;
        iss >> std::noskipws >> value;
        return iss.eof() && !iss.fail();
    }

    long TypeConverter::toLong(const std::string& str) {
        #ifdef URCHIN_DEBUG
            assert(isLong(str));
        #endif
        std::istringstream iss(str);
        long value = 0;
        iss >> value;
        return value;
    }

    bool TypeConverter::isUint32(const std::string& str) {
        std::istringstream iss(str);
        uint64_t value = 0;
        iss >> std::noskipws >> value;
        return iss.eof() && !iss.fail();
    }

    uint32_t TypeConverter::toUint32(const std::string& str) {
        #ifdef URCHIN_DEBUG
            assert(isUint32(str));
        #endif
        std::istringstream iss(str);
        uint32_t value = 0;
        iss >> value;
        return value;
    }

    bool TypeConverter::isUint64(const std::string& str) {
        std::istringstream iss(str);
        uint64_t value = 0;
        iss >> std::noskipws >> value;
        return iss.eof() && !iss.fail();
    }

    uint64_t TypeConverter::toUint64(const std::string& str) {
        #ifdef URCHIN_DEBUG
            assert(isUint64(str));
        #endif
        std::istringstream iss(str);
        uint64_t value = 0;
        iss >> value;
        return value;
    }

    bool TypeConverter::isUnsignedInt(const std::string& str) {
        std::istringstream iss(str);
        unsigned int value = 0;
        iss >> std::noskipws >> value;
        return iss.eof() && !iss.fail();
    }

    unsigned int TypeConverter::toUnsignedInt(const std::string& str) {
        #ifdef URCHIN_DEBUG
            assert(isUnsignedInt(str));
        #endif
        std::istringstream iss(str);
        unsigned int value = 0;
        iss >> value;
        return value;
    }

    bool TypeConverter::isSize(const std::string& str) {
        std::istringstream iss(str);
        std::size_t value = 0;
        iss >> std::noskipws >> value;
        return iss.eof() && !iss.fail();
    }

    std::size_t TypeConverter::toSize(const std::string& str) {
        #ifdef URCHIN_DEBUG
            assert(isSize(str));
        #endif
        std::istringstream iss(str);
        std::size_t value = 0;
        iss >> value;
        return value;
    }

    bool TypeConverter::isFloat(const std::string& str) {
        std::istringstream iss(str);
        iss.imbue(std::locale::classic());
        float value = 0.0f;
        iss >> std::noskipws >> value;
        return iss.eof() && !iss.fail();
    }

    float TypeConverter::toFloat(const std::string& str) {
        #ifdef URCHIN_DEBUG
            assert(isFloat(str));
        #endif
        std::istringstream iss(str);
        iss.imbue(std::locale::classic());
        float value = 0.0f;
        iss >> value;
        return value;
    }

    bool TypeConverter::isDouble(const std::string& str) {
        std::istringstream iss(str);
        iss.imbue(std::locale::classic());
        double value = 0.0;
        iss >> std::noskipws >> value;
        return iss.eof() && !iss.fail();
    }

    double TypeConverter::toDouble(const std::string& str) {
        #ifdef URCHIN_DEBUG
            assert(isDouble(str));
        #endif
        std::istringstream iss(str);
        iss.imbue(std::locale::classic());
        double value = 0.0;
        iss >> value;
        return value;
    }

    bool TypeConverter::isChar(std::string_view str) {
        return str.size() == 1;
    }

    char TypeConverter::toChar(std::string_view str) {
        #ifdef URCHIN_DEBUG
            assert(isChar(str));
        #endif
        return str[0];
    }

    bool TypeConverter::isBool(const std::string& str) {
        return str.size() == 1 && isInt(str);
    }

    bool TypeConverter::toBool(const std::string& str) {
        #ifdef URCHIN_DEBUG
            assert(isBool(str));
        #endif
        return str[0] != '0';
    }

    Point2<float> TypeConverter::toPoint2(std::string_view str) {
        std::vector<float> floatValues = floatSplit(str, 2);
        return {floatValues[0], floatValues[1]};
    }

    Point2<int> TypeConverter::toPoint2Int(std::string_view str) {
        std::vector<int> intValues = intSplit(str, 2);
        return {intValues[0], intValues[1]};
    }

    Point3<float> TypeConverter::toPoint3(std::string_view str) {
        std::vector<float> floatValues = floatSplit(str, 3);
        return {floatValues[0], floatValues[1], floatValues[2]};
    }

    Point3<int> TypeConverter::toPoint3Int(std::string_view str) {
        std::vector<int> intValues = intSplit(str, 3);
        return {intValues[0], intValues[1], intValues[2]};
    }

    Point4<float> TypeConverter::toPoint4(std::string_view str) {
        std::vector<float> floatValues = floatSplit(str, 4);
        return {floatValues[0], floatValues[1], floatValues[2], floatValues[3]};
    }

    Point4<int> TypeConverter::toPoint4Int(std::string_view str) {
        std::vector<int> intValues = intSplit(str, 4);
        return {intValues[0], intValues[1], intValues[2], intValues[3]};
    }

    Vector2<float> TypeConverter::toVector2(std::string_view str) {
        std::vector<float> floatValues = floatSplit(str, 2);
        return {floatValues[0], floatValues[1]};
    }

    Vector2<int> TypeConverter::toVector2Int(std::string_view str) {
        std::vector<int> intValues = intSplit(str, 2);
        return {intValues[0], intValues[1]};
    }

    Vector3<float> TypeConverter::toVector3(std::string_view str) {
        std::vector<float> floatValues = floatSplit(str, 3);
        return {floatValues[0], floatValues[1], floatValues[2]};
    }

    Vector3<int> TypeConverter::toVector3Int(std::string_view str) {
        std::vector<int> intValues = intSplit(str, 3);
        return {intValues[0], intValues[1], intValues[2]};
    }

    Vector4<float> TypeConverter::toVector4(std::string_view str) {
        std::vector<float> floatValues = floatSplit(str, 4);
        return {floatValues[0], floatValues[1], floatValues[2], floatValues[3]};
    }

    Vector4<int> TypeConverter::toVector4Int(std::string_view str) {
        std::vector<int> intValues = intSplit(str, 4);
        return {intValues[0], intValues[1], intValues[2], intValues[3]};
    }

    Quaternion<float> TypeConverter::toQuaternion(std::string_view str) {
        std::vector<float> floatValues = floatSplit(str, 4);
        return {floatValues[0], floatValues[1], floatValues[2], floatValues[3]};
    }

    long long TypeConverter::toLongLong(float value, float scale) {
        const float minValue = (float)std::numeric_limits<long long>::min() / scale;
        const float maxValue = (float)std::numeric_limits<long long>::max() / scale;

        if (value < 0.0) {
            if (value < minValue) [[unlikely]] {
                Logger::instance().logError("Impossible to convert float " + std::to_string(value) + " in long long type.");
            }
            return std::llround(value * scale - 0.5);
        }
        else {
            if (value > maxValue) [[unlikely]] {
                Logger::instance().logError("Impossible to convert float " + std::to_string(value) + " in long long type.");
            }
            return std::llround(value * scale + 0.5);
        }
    }

    float TypeConverter::toFloat(long long value, float scale) {
        return (float)value / scale;
    }

    /**
     * Convert float to string. Unlike std::to_string(), this method is independent of the locale.
     */
    std::string TypeConverter::toString(float value) {
        std::ostringstream ss;
        ss.imbue(std::locale::classic());
        ss << std::setprecision(std::numeric_limits<float>::digits10) << std::fixed << value;
        return std::string(ss.str());
    }

    /**
     * Convert double to string. Unlike std::to_string(), this method is independent of the locale.
     */
    std::string TypeConverter::toString(double value) {
        std::ostringstream ss;
        ss.imbue(std::locale::classic());
        ss << std::setprecision(std::numeric_limits<double>::digits10) << std::fixed << value;
        return std::string(ss.str());
    }

    std::string TypeConverter::toString(int value) {
        return std::to_string(value);
    }

    std::vector<float> TypeConverter::floatSplit(std::string_view str, unsigned int expectedSplit) {
        std::vector<std::string> stringValues;
        stringValues.reserve(expectedSplit);
        StringUtil::split(str, NUMBER_DELIMITER, stringValues);

        if (stringValues.size() != expectedSplit) [[unlikely]] {
            throw std::invalid_argument("Number of float expected: " + std::to_string(expectedSplit) + ", found: "
                    + std::to_string((unsigned int)stringValues.size()) + ". String value: " + std::string(str) + ".");
        }

        std::vector<float> floatValues;
        floatValues.reserve(expectedSplit);
        for (const auto& stringValue : stringValues) {
            floatValues.push_back(toFloat(stringValue));
        }

        return floatValues;
    }

    std::vector<int> TypeConverter::intSplit(std::string_view str, unsigned int expectedSplit) {
        std::vector<std::string> stringValues;
        stringValues.reserve(expectedSplit);
        StringUtil::split(str, NUMBER_DELIMITER, stringValues);

        if (stringValues.size() != expectedSplit) [[unlikely]] {
            throw std::invalid_argument("Number of int expected: " + std::to_string(expectedSplit) + ", found: "
                                        + std::to_string((unsigned int)stringValues.size()) + ". String value: " + std::string(str) + ".");
        }

        std::vector<int> intValues;
        intValues.reserve(expectedSplit);
        for (const auto& stringValue : stringValues) {
            intValues.push_back(toInt(stringValue));
        }

        return intValues;
    }

}
