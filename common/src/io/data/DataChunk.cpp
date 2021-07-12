#include <stdexcept>

#include <io/data/DataChunk.h>
#include <util/TypeConverter.h>

namespace urchin {

    DataChunk::DataChunk(DataContentLine& chunk) :
            chunk(chunk) {

    }

    DataChunk::DataChunk(const DataContentLine& chunk) :
            DataChunk(const_cast<DataContentLine&>(chunk)) {

    }

    DataContentLine& DataChunk::getChunk() const {
        return chunk;
    }

    /**
     * @return Attribute value if exist otherwise an empty string
     */
    std::string DataChunk::getAttributeValue(const std::string& attributeName) const {
        const auto& attributes = chunk.getAttributes();
        auto itFind = attributes.find(attributeName);
        if (itFind != attributes.end()) {
            return itFind->second;
        }
        return "";
    }

    void DataChunk::setAttribute(const DataAttribute& /*attribute*/) {
        //TODO chunk->SetAttribute(attribute.getAttributeName(), attribute.getAttributeValue());
    }

    std::string DataChunk::getStringValue() const {
        std::string value = chunk.getValue();
        if(value.empty()) { //TODO diff between null & empty ! Use optional !
            throw std::domain_error("Impossible to find a value on chunk: " + chunk.getName());
        }
        return value;
    }

    void DataChunk::setStringValue(const std::string& /*value*/) {
        //TODO chunk->LinkEndChild(new TiXmlText(value));
    }

    int DataChunk::getIntValue() const {
        return TypeConverter::toInt(getStringValue());
    }

    void DataChunk::setIntValue(int value) {
        setStringValue(std::to_string(value));
    }

    unsigned int DataChunk::getUnsignedIntValue() const {
        return TypeConverter::toUnsignedInt(getStringValue());
    }

    void DataChunk::setUnsignedIntValue(unsigned int value) {
        setStringValue(std::to_string(value));
    }

    float DataChunk::getFloatValue() const {
        return TypeConverter::toFloat(getStringValue());
    }

    void DataChunk::setFloatValue(float value) {
        std::locale::global(std::locale("C")); //for float

        setStringValue(std::to_string(value));
    }

    bool DataChunk::getBoolValue() const {
        std::string value = getStringValue();
        if (value == "false") {
            return false;
        } else if (value == "true") {
            return true;
        }

        throw std::domain_error("Impossible to convert " + value + " into a boolean");
    }

    void DataChunk::setBoolValue(bool value) {
        if (value) {
            setStringValue("true");
        } else {
            setStringValue("false");
        }
    }

    char DataChunk::getCharValue() const {
        return TypeConverter::toChar(getStringValue());
    }

    void DataChunk::setCharValue(char value) {
        setStringValue(std::to_string(value));
    }

    Point2<float> DataChunk::getPoint2Value() const {
        return TypeConverter::toPoint2(getStringValue());
    }

    void DataChunk::setPoint2Value(const Point2<float>& value) {
        std::locale::global(std::locale("C")); //for float

        setStringValue(std::to_string(value.X) + TypeConverter::FLOAT_DELIMITER + std::to_string(value.Y));
    }

    Point3<float> DataChunk::getPoint3Value() const {
        return TypeConverter::toPoint3(getStringValue());
    }

    void DataChunk::setPoint3Value(const Point3<float>& value) {
        std::locale::global(std::locale("C")); //for float

        setStringValue(std::to_string(value.X) + TypeConverter::FLOAT_DELIMITER + std::to_string(value.Y) + TypeConverter::FLOAT_DELIMITER + std::to_string(value.Z));
    }

    Point4<float> DataChunk::getPoint4Value() const {
        return TypeConverter::toPoint4(getStringValue());
    }

    void DataChunk::setPoint4Value(const Point4<float>& value) {
        std::locale::global(std::locale("C")); //for float

        setStringValue(std::to_string(value.X) + TypeConverter::FLOAT_DELIMITER + std::to_string(value.Y) + TypeConverter::FLOAT_DELIMITER + std::to_string(value.Z)
            + TypeConverter::FLOAT_DELIMITER + std::to_string(value.W));
    }

    Vector2<float> DataChunk::getVector2Value() const {
        return TypeConverter::toVector2(getStringValue());
    }

    void DataChunk::setVector2Value(const Vector2<float>& value) {
        std::locale::global(std::locale("C")); //for float

        setStringValue(std::to_string(value.X) + TypeConverter::FLOAT_DELIMITER + std::to_string(value.Y));
    }

    Vector3<float> DataChunk::getVector3Value() const {
        return TypeConverter::toVector3(getStringValue());
    }

    void DataChunk::setVector3Value(const Vector3<float>& value) {
        std::locale::global(std::locale("C")); //for float

        setStringValue(std::to_string(value.X) + TypeConverter::FLOAT_DELIMITER + std::to_string(value.Y) + TypeConverter::FLOAT_DELIMITER + std::to_string(value.Z));
    }

    Vector4<float> DataChunk::getVector4Value() const {
        return TypeConverter::toVector4(getStringValue());
    }

    void DataChunk::setVector4Value(const Vector4<float>& value) {
        std::locale::global(std::locale("C")); //for float

        setStringValue(std::to_string(value.X) + TypeConverter::FLOAT_DELIMITER + std::to_string(value.Y) + TypeConverter::FLOAT_DELIMITER + std::to_string(value.Z)
            + TypeConverter::FLOAT_DELIMITER + std::to_string(value.W));
    }
}
