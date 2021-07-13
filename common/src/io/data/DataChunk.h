#pragma once

#include <string>
#include <vector>
#include <map>

#include <io/data/DataAttribute.h>
#include <math/algebra/point/Point2.h>
#include <math/algebra/point/Point3.h>
#include <math/algebra/point/Point4.h>
#include <math/algebra/vector/Vector2.h>
#include <math/algebra/vector/Vector3.h>
#include <math/algebra/vector/Vector4.h>

namespace urchin {

    class DataChunk {
        public:
            static const unsigned int INDENT_SPACES;
            static const char ATTRIBUTES_SEPARATOR;
            static const char ATTRIBUTES_ASSIGN;

            DataChunk(std::string, std::string, std::map<std::string, std::string>, DataChunk*);

            const std::string& getName() const;

            const std::map<std::string, std::string>& getAttributes() const;
            std::string getAttributeValue(const std::string&) const;
            void addAttribute(const DataAttribute&);

            DataChunk* getParent() const;
            DataChunk& addChild(std::unique_ptr<DataChunk>);
            const std::vector<std::unique_ptr<DataChunk>>& getChildren() const;

            std::string getStringValue() const;
            void setStringValue(const std::string&);

            int getIntValue() const;
            void setIntValue(int);

            unsigned int getUnsignedIntValue() const;
            void setUnsignedIntValue(unsigned int);

            float getFloatValue() const;
            void setFloatValue(float);

            bool getBoolValue() const;
            void setBoolValue(bool);

            char getCharValue() const;
            void setCharValue(char);

            Point2<float> getPoint2Value() const;
            void setPoint2Value(const Point2<float>&);

            Point3<float> getPoint3Value() const;
            void setPoint3Value(const Point3<float>&);

            Point4<float> getPoint4Value() const;
            void setPoint4Value(const Point4<float>&);

            Vector2<float> getVector2Value() const;
            void setVector2Value(const Vector2<float>&);

            Vector3<float> getVector3Value() const;
            void setVector3Value(const Vector3<float>&);

            Vector4<float> getVector4Value() const;
            void setVector4Value(const Vector4<float>&);

        private:
            std::string name;
            std::string value;
            std::map<std::string, std::string> attributes;

            DataChunk* parent;
            std::vector<std::unique_ptr<DataChunk>> children;
    };

}
