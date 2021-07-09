#pragma once

#include <string>
#include <vector>

#include <libs/tinyxml/tinyxml.h>
#include <io/data/DataAttribute.h>
#include <math/algebra/point/Point2.h>
#include <math/algebra/point/Point3.h>
#include <math/algebra/point/Point4.h>
#include <math/algebra/vector/Vector2.h>
#include <math/algebra/vector/Vector3.h>
#include <math/algebra/vector/Vector4.h>

namespace urchin {

    class XmlChunk {
        public:
            friend class DataParser;
            friend class DataWriter;

            std::string getAttributeValue(const std::string&) const;
            void setAttribute(const DataAttribute&);

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
            explicit XmlChunk(TiXmlElement*);
            explicit XmlChunk(const TiXmlElement*);

            TiXmlElement* getChunk() const;

            TiXmlElement* chunk;
    };

}
