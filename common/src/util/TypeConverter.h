#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <math/algebra/point/Point2.h>
#include <math/algebra/point/Point3.h>
#include <math/algebra/point/Point4.h>
#include <math/algebra/vector/Vector2.h>
#include <math/algebra/vector/Vector3.h>
#include <math/algebra/vector/Vector4.h>
#include <math/algebra/Quaternion.h>

namespace urchin {

    class TypeConverter {
        public:
            static constexpr char NUMBER_DELIMITER = ' ';
            static constexpr float FLOAT_INT_SCALE = 8192.0f;

            TypeConverter() = delete;

            static bool isInt(const std::string&);
            static int toInt(const std::string&);
            static bool isLong(const std::string&);
            static long toLong(const std::string&);
            static bool isUint32(const std::string&);
            static uint32_t toUint32(const std::string&);
            static bool isUint64(const std::string&);
            static uint64_t toUint64(const std::string&);
            static bool isUnsignedInt(const std::string&);
            static unsigned int toUnsignedInt(const std::string&);
            static bool isSize(const std::string&);
            static std::size_t toSize(const std::string&);
            static bool isFloat(const std::string&);
            static float toFloat(const std::string&);
            static bool isDouble(const std::string&);
            static double toDouble(const std::string&);
            static bool isChar(std::string_view);
            static char toChar(std::string_view);
            static bool isBool(const std::string&);
            static bool toBool(const std::string&);
            static Point2<float> toPoint2(std::string_view);
            static Point2<int> toPoint2Int(std::string_view);
            static Point3<float> toPoint3(std::string_view);
            static Point3<int> toPoint3Int(std::string_view);
            static Point4<float> toPoint4(std::string_view);
            static Point4<int> toPoint4Int(std::string_view);
            static Vector2<float> toVector2(std::string_view);
            static Vector2<int> toVector2Int(std::string_view);
            static Vector3<float> toVector3(std::string_view);
            static Vector3<int> toVector3Int(std::string_view);
            static Vector4<float> toVector4(std::string_view);
            static Vector4<int> toVector4Int(std::string_view);
            static Quaternion<float> toQuaternion(std::string_view);

            static long long toLongLong(float, float scale = FLOAT_INT_SCALE);
            static float toFloat(long long, float scale = FLOAT_INT_SCALE);

            static std::string toString(float);
            static std::string toString(double);
            static std::string toString(int);

        private:
            static std::vector<float> floatSplit(std::string_view, unsigned int);
            static std::vector<int> intSplit(std::string_view, unsigned int);
    };

}
