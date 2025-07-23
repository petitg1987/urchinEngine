#include <stdexcept>

#include "io/uda/UdaChunk.h"
#include "util/TypeConverter.h"

namespace urchin {

    UdaChunk::UdaChunk(std::string name, std::string value, std::map<std::string, std::string, std::less<>> attributes, UdaChunk* parent):
            name(std::move(name)),
            value(std::move(value)),
            attributes(std::move(attributes)),
            parent(parent) {

    }

    UdaChunk::UdaChunk(const UdaChunk& chunk) :
            name(chunk.name),
            value(chunk.value),
            attributes(chunk.attributes),
            parent(nullptr) {
        for (const std::unique_ptr<UdaChunk>& child : chunk.children) {
            children.push_back(std::make_unique<UdaChunk>(*child));
            children.back()->parent = this;
        }
    }

    const std::string& UdaChunk::getName() const {
        return name;
    }

    const std::map<std::string, std::string, std::less<>>& UdaChunk::getAttributes() const {
        return attributes;
    }

    std::string UdaChunk::getAttributeValue(std::string_view attributeName) const {
        auto itFind = attributes.find(attributeName);
        if (itFind != attributes.end()) {
            return itFind->second;
        }
        return "";
    }

    void UdaChunk::addAttribute(const UdaAttribute& attribute) {
        attributes.try_emplace(attribute.getAttributeName(), attribute.getAttributeValue());
    }

    UdaChunk* UdaChunk::getParent() const {
        return parent;
    }

    UdaChunk& UdaChunk::addChild(std::unique_ptr<UdaChunk> child) {
        children.push_back(std::move(child));
        return *children.back();
    }

    void UdaChunk::removeChild(const UdaChunk& chunk) {
        std::erase_if(children, [&chunk](const std::unique_ptr<UdaChunk> &node){ return &chunk == node.get(); });
    }

    const std::vector<std::unique_ptr<UdaChunk>>& UdaChunk::getChildren() const {
        return children;
    }

    const std::string& UdaChunk::getStringValue() const {
        return value;
    }

    void UdaChunk::setStringValue(std::string value) {
        this->value = std::move(value);
    }

    int UdaChunk::getIntValue() const {
        return TypeConverter::toInt(getStringValue());
    }

    void UdaChunk::setIntValue(int value) {
        setStringValue(std::to_string(value));
    }

    long UdaChunk::getLongValue() const {
        return TypeConverter::toLong(getStringValue());
    }

    void UdaChunk::setLongValue(long value) {
        setStringValue(std::to_string(value));
    }

    uint64_t UdaChunk::getUint64Value() const {
        return TypeConverter::toUint64(getStringValue());
    }

    void UdaChunk::setUint64Value(uint64_t value) {
        setStringValue(std::to_string(value));
    }

    unsigned int UdaChunk::getUnsignedIntValue() const {
        return TypeConverter::toUnsignedInt(getStringValue());
    }

    void UdaChunk::setUnsignedIntValue(unsigned int value) {
        setStringValue(std::to_string(value));
    }

    float UdaChunk::getFloatValue() const {
        return TypeConverter::toFloat(getStringValue());
    }

    void UdaChunk::setFloatValue(float value) {
        setStringValue(floatToString(value));
    }

    double UdaChunk::getDoubleValue() const {
        return TypeConverter::toDouble(getStringValue());
    }

    void UdaChunk::setDoubleValue(double value) {
        setStringValue(doubleToString(value));
    }

    bool UdaChunk::getBoolValue() const {
        std::string value = getStringValue();
        if (value == "false") {
            return false;
        } else if (value == "true") {
            return true;
        }

        throw std::domain_error("Impossible to convert " + value + " into a boolean");
    }

    void UdaChunk::setBoolValue(bool value) {
        if (value) {
            setStringValue("true");
        } else {
            setStringValue("false");
        }
    }

    char UdaChunk::getCharValue() const {
        return TypeConverter::toChar(getStringValue());
    }

    void UdaChunk::setCharValue(char value) {
        setStringValue(std::to_string(value));
    }

    Point2<float> UdaChunk::getPoint2Value() const {
        return TypeConverter::toPoint2(getStringValue());
    }

    void UdaChunk::setPoint2Value(const Point2<float>& value) {
        setStringValue(
                floatToString(value.X) + TypeConverter::NUMBER_DELIMITER +
                floatToString(value.Y));
    }

    Point2<int> UdaChunk::getPoint2IntValue() const {
        return TypeConverter::toPoint2Int(getStringValue());
    }

    void UdaChunk::setPoint2IntValue(const Point2<int>& value) {
        setStringValue(
                TypeConverter::toString(value.X) + TypeConverter::NUMBER_DELIMITER +
                TypeConverter::toString(value.Y));
    }

    Point3<float> UdaChunk::getPoint3Value() const {
        return TypeConverter::toPoint3(getStringValue());
    }

    void UdaChunk::setPoint3Value(const Point3<float>& value) {
        setStringValue(
                floatToString(value.X) + TypeConverter::NUMBER_DELIMITER +
                floatToString(value.Y) + TypeConverter::NUMBER_DELIMITER +
                floatToString(value.Z));
    }

    Point3<int> UdaChunk::getPoint3IntValue() const {
        return TypeConverter::toPoint3Int(getStringValue());
    }

    void UdaChunk::setPoint3IntValue(const Point3<int>& value) {
        setStringValue(
                TypeConverter::toString(value.X) + TypeConverter::NUMBER_DELIMITER +
                TypeConverter::toString(value.Y) + TypeConverter::NUMBER_DELIMITER +
                TypeConverter::toString(value.Z));
    }

    Point4<float> UdaChunk::getPoint4Value() const {
        return TypeConverter::toPoint4(getStringValue());
    }

    void UdaChunk::setPoint4Value(const Point4<float>& value) {
        setStringValue(
                floatToString(value.X) + TypeConverter::NUMBER_DELIMITER +
                floatToString(value.Y) + TypeConverter::NUMBER_DELIMITER +
                floatToString(value.Z) + TypeConverter::NUMBER_DELIMITER +
                floatToString(value.W));
    }

    Point4<int> UdaChunk::getPoint4IntValue() const {
        return TypeConverter::toPoint4Int(getStringValue());
    }

    void UdaChunk::setPoint4IntValue(const Point4<int>& value) {
        setStringValue(
                TypeConverter::toString(value.X) + TypeConverter::NUMBER_DELIMITER +
                TypeConverter::toString(value.Y) + TypeConverter::NUMBER_DELIMITER +
                TypeConverter::toString(value.Z) + TypeConverter::NUMBER_DELIMITER +
                TypeConverter::toString(value.W));
    }

    Vector2<float> UdaChunk::getVector2Value() const {
        return TypeConverter::toVector2(getStringValue());
    }

    void UdaChunk::setVector2Value(const Vector2<float>& value) {
        setStringValue(
                floatToString(value.X) + TypeConverter::NUMBER_DELIMITER +
                floatToString(value.Y));
    }

    Vector2<int> UdaChunk::getVector2IntValue() const {
        return TypeConverter::toVector2Int(getStringValue());
    }

    void UdaChunk::setVector2IntValue(const Vector2<int>& value) {
        setStringValue(
                TypeConverter::toString(value.X) + TypeConverter::NUMBER_DELIMITER +
                TypeConverter::toString(value.Y));
    }

    Vector3<float> UdaChunk::getVector3Value() const {
        return TypeConverter::toVector3(getStringValue());
    }

    void UdaChunk::setVector3Value(const Vector3<float>& value) {
        setStringValue(
                floatToString(value.X) + TypeConverter::NUMBER_DELIMITER +
                floatToString(value.Y) + TypeConverter::NUMBER_DELIMITER +
                floatToString(value.Z));
    }

    Vector3<int> UdaChunk::getVector3IntValue() const {
        return TypeConverter::toVector3Int(getStringValue());
    }

    void UdaChunk::setVector3IntValue(const Vector3<int>& value) {
        setStringValue(
                TypeConverter::toString(value.X) + TypeConverter::NUMBER_DELIMITER +
                TypeConverter::toString(value.Y) + TypeConverter::NUMBER_DELIMITER +
                TypeConverter::toString(value.Z));
    }

    Vector4<float> UdaChunk::getVector4Value() const {
        return TypeConverter::toVector4(getStringValue());
    }

    void UdaChunk::setVector4Value(const Vector4<float>& value) {
        setStringValue(
                floatToString(value.X) + TypeConverter::NUMBER_DELIMITER +
                floatToString(value.Y) + TypeConverter::NUMBER_DELIMITER +
                floatToString(value.Z) + TypeConverter::NUMBER_DELIMITER +
                floatToString(value.W));
    }

    Vector4<int> UdaChunk::getVector4IntValue() const {
        return TypeConverter::toVector4Int(getStringValue());
    }

    void UdaChunk::setVector4IntValue(const Vector4<int>& value) {
        setStringValue(
                TypeConverter::toString(value.X) + TypeConverter::NUMBER_DELIMITER +
                TypeConverter::toString(value.Y) + TypeConverter::NUMBER_DELIMITER +
                TypeConverter::toString(value.Z) + TypeConverter::NUMBER_DELIMITER +
                TypeConverter::toString(value.W));
    }

    Quaternion<float> UdaChunk::getQuaternionValue() const {
        return TypeConverter::toQuaternion(getStringValue());
    }

    void UdaChunk::setQuaternionValue(const Quaternion<float>& value) {
        setStringValue(
                floatToString(value.X) + TypeConverter::NUMBER_DELIMITER +
                floatToString(value.Y) + TypeConverter::NUMBER_DELIMITER +
                floatToString(value.Z) + TypeConverter::NUMBER_DELIMITER +
                floatToString(value.W));
    }

    std::string UdaChunk::floatToString(float value) const {
        std::string floatStr = TypeConverter::toString(value);
        removeTrailingZero(floatStr);
        return floatStr;
    }

    std::string UdaChunk::doubleToString(double value) const {
        std::string doubleStr = TypeConverter::toString(value);
        removeTrailingZero(doubleStr);
        return doubleStr;
    }

    void UdaChunk::removeTrailingZero(std::string& str) const {
        if (str.find('.') != std::string::npos) {
            str = str.substr(0, str.find_last_not_of('0') + 1);
            if (str.find('.') == str.size() - 1) {
                str = str.substr(0, str.size() - 1);
            }
        }
    }
}
