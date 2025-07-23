#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "io/uda/UdaAttribute.h"
#include "math/algebra/point/Point2.h"
#include "math/algebra/point/Point3.h"
#include "math/algebra/point/Point4.h"
#include "math/algebra/vector/Vector2.h"
#include "math/algebra/vector/Vector3.h"
#include "math/algebra/vector/Vector4.h"
#include "math/algebra/Quaternion.h"

namespace urchin {

    class UdaChunk {
        public:
            static constexpr unsigned int INDENT_SPACES = 2;
            static constexpr char ATTRIBUTES_SEPARATOR = ';';
            static constexpr char ATTRIBUTES_ASSIGN = '=';

            UdaChunk(std::string, std::string, std::map<std::string, std::string, std::less<>>, UdaChunk*);
            UdaChunk(const UdaChunk&);
            ~UdaChunk() = default;
            UdaChunk& operator=(const UdaChunk&) = delete;
            UdaChunk(UdaChunk&&) noexcept = default;
            UdaChunk& operator=(UdaChunk&&) noexcept = delete;

            const std::string& getName() const;

            const std::map<std::string, std::string, std::less<>>& getAttributes() const;
            std::string getAttributeValue(std::string_view) const;
            void addAttribute(const UdaAttribute&);

            UdaChunk* getParent() const;
            UdaChunk& addChild(std::unique_ptr<UdaChunk>);
            void removeChild(const UdaChunk&);
            const std::vector<std::unique_ptr<UdaChunk>>& getChildren() const;

            const std::string& getStringValue() const;
            void setStringValue(std::string);

            int getIntValue() const;
            void setIntValue(int);

            long getLongValue() const;
            void setLongValue(long);

            uint64_t getUint64Value() const;
            void setUint64Value(uint64_t);

            unsigned int getUnsignedIntValue() const;
            void setUnsignedIntValue(unsigned int);

            float getFloatValue() const;
            void setFloatValue(float);

            double getDoubleValue() const;
            void setDoubleValue(double);

            bool getBoolValue() const;
            void setBoolValue(bool);

            char getCharValue() const;
            void setCharValue(char);

            Point2<float> getPoint2Value() const;
            void setPoint2Value(const Point2<float>&);
            Point2<int> getPoint2IntValue() const;
            void setPoint2IntValue(const Point2<int>&);

            Point3<float> getPoint3Value() const;
            void setPoint3Value(const Point3<float>&);
            Point3<int> getPoint3IntValue() const;
            void setPoint3IntValue(const Point3<int>&);

            Point4<float> getPoint4Value() const;
            void setPoint4Value(const Point4<float>&);
            Point4<int> getPoint4IntValue() const;
            void setPoint4IntValue(const Point4<int>&);

            Vector2<float> getVector2Value() const;
            void setVector2Value(const Vector2<float>&);
            Vector2<int> getVector2IntValue() const;
            void setVector2IntValue(const Vector2<int>&);

            Vector3<float> getVector3Value() const;
            void setVector3Value(const Vector3<float>&);
            Vector3<int> getVector3IntValue() const;
            void setVector3IntValue(const Vector3<int>&);

            Vector4<float> getVector4Value() const;
            void setVector4Value(const Vector4<float>&);
            Vector4<int> getVector4IntValue() const;
            void setVector4IntValue(const Vector4<int>&);

            Quaternion<float> getQuaternionValue() const;
            void setQuaternionValue(const Quaternion<float>&);

        private:
            std::string floatToString(float) const;
            std::string doubleToString(double) const;
            void removeTrailingZero(std::string&) const;

            std::string name;
            std::string value;
            std::map<std::string, std::string, std::less<>> attributes;

            UdaChunk* parent;
            std::vector<std::unique_ptr<UdaChunk>> children;
    };

}
