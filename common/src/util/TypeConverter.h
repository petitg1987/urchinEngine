#pragma once

#include <math/algebra/point/Point2.h>
#include <math/algebra/point/Point3.h>
#include <math/algebra/point/Point4.h>
#include <math/algebra/vector/Vector2.h>
#include <math/algebra/vector/Vector3.h>
#include <math/algebra/vector/Vector4.h>
#include <math/algebra/Quaternion.h>

#include <string>
#include <vector>

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
            static bool isChar(const std::string&);
            static char toChar(const std::string&);
            static bool isBool(const std::string&);
            static bool toBool(const std::string&);
            static Point2<float> toPoint2(const std::string&);
            static Point2<int> toPoint2Int(const std::string&);
            static Point3<float> toPoint3(const std::string&);
            static Point3<int> toPoint3Int(const std::string&);
            static Point4<float> toPoint4(const std::string&);
            static Point4<int> toPoint4Int(const std::string&);
            static Vector2<float> toVector2(const std::string&);
            static Vector2<int> toVector2Int(const std::string&);
            static Vector3<float> toVector3(const std::string&);
            static Vector3<int> toVector3Int(const std::string&);
            static Vector4<float> toVector4(const std::string&);
            static Vector4<int> toVector4Int(const std::string&);
            static Quaternion<float> toQuaternion(const std::string&);

            static long long toLongLong(float, float scale = FLOAT_INT_SCALE);
            static float toFloat(long long, float scale = FLOAT_INT_SCALE);

            static std::string toString(float);
            static std::string toString(double);
            static std::string toString(int);

        private:
            static std::vector<float> floatSplit(const std::string&, unsigned int);
            static std::vector<int> intSplit(const std::string&, unsigned int);
    };

}
