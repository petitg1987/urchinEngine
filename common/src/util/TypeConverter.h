#ifndef URCHINENGINE_CONVERTER_H
#define URCHINENGINE_CONVERTER_H

#include "pattern/singleton/Singleton.h"
#include "math/algebra/point/Point2.h"
#include "math/algebra/point/Point3.h"
#include "math/algebra/point/Point4.h"
#include "math/algebra/vector/Vector2.h"
#include "math/algebra/vector/Vector3.h"
#include "math/algebra/vector/Vector4.h"

#include <string>
#include <vector>

namespace urchin {

    class TypeConverter : public Singleton<TypeConverter> {
        public:
            friend class Singleton<TypeConverter>;

            static const char FLOAT_DELIMITER;
            static const float FLOAT_INT_SCALE;

            bool isInt(const std::string&);
            int toInt(const std::string&);
            bool isUnsignedInt(const std::string&);
            unsigned int toUnsignedInt(const std::string&);
            bool isFloat(const std::string&);
            float toFloat(const std::string&);
            bool isChar(const std::string&);
            char toChar(const std::string&);

            long long toLongLong(float, float scale = FLOAT_INT_SCALE);
            float toFloat(long long, float scale = FLOAT_INT_SCALE);

            Point2<float> toPoint2(const std::string&);
            Point3<float> toPoint3(const std::string&);
            Point4<float> toPoint4(const std::string&);
            Vector2<float> toVector2(const std::string&);
            Vector3<float> toVector3(const std::string&);
            Vector4<float> toVector4(const std::string&);

        private:
            TypeConverter() = default;
            ~TypeConverter() override = default;

            std::vector<float> floatSplit(const std::string&, unsigned int);
    };

}

#endif
