#ifndef URCHINENGINE_CONVERTER_H
#define URCHINENGINE_CONVERTER_H

#include "math/algebra/point/Point2.h"
#include "math/algebra/point/Point3.h"
#include "math/algebra/point/Point4.h"
#include "math/algebra/vector/Vector2.h"
#include "math/algebra/vector/Vector3.h"
#include "math/algebra/vector/Vector4.h"

#include <string>
#include <vector>

namespace urchin {

    class Converter {
        public:
            static const char FLOAT_DELIMITER;
            static const float FLOAT_INT_SCALE;

            static bool isInt(const std::string &);
            static int toInt(const std::string &);
            static bool isUnsignedInt(const std::string &);
            static unsigned int toUnsignedInt(const std::string &);
            static bool isFloat(const std::string &);
            static float toFloat(const std::string &);
            static bool isChar(const std::string &);
            static char toChar(const std::string &);

            static long long toLongLong(float, float scale=FLOAT_INT_SCALE);
            static float toFloat(long long, float scale=FLOAT_INT_SCALE);

            static Point2<float> toPoint2(const std::string &);
            static Point3<float> toPoint3(const std::string &);
            static Point4<float> toPoint4(const std::string &);
            static Vector2<float> toVector2(const std::string &);
            static Vector3<float> toVector3(const std::string &);
            static Vector4<float> toVector4(const std::string &);

        private:
            Converter() = default;
            ~Converter() = default;

            static std::vector<float> floatSplit(const std::string &, unsigned int);
    };

}

#endif
