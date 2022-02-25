template<class T> std::shared_ptr<T> Widget::create(T* widget, Widget* parent) {
    std::shared_ptr<T> widgetPtr = std::shared_ptr<T>(widget);
    if (parent) {
        parent->addChild(widgetPtr);
    }
    return widgetPtr;
}

template<class T> int Widget::widthLengthToPixel(float widthValue, LengthType lengthType, const T& heightValueInPixel) const {
    if (lengthType == LengthType::SCREEN_PERCENT) {
        return MathFunction::roundToInt(widthValue / 100.0f * (float)getSceneSize().X);
    } else if (lengthType == LengthType::CONTAINER_PERCENT) {
        if (!getParentContainer()) {
            throw std::runtime_error("Missing parent container on the widget");
        }
        return MathFunction::roundToInt(widthValue / 100.0f * (float) ((Widget*) getParentContainer())->getWidth());
    } else if (lengthType == LengthType::PARENT_PERCENT) {
        if (!getParent()) {
            throw std::runtime_error("Missing parent on the widget");
        }
        return MathFunction::roundToInt(widthValue / 100.0f * (float) getParent()->getWidth());
    } else if (lengthType == LengthType::RATIO_TO_HEIGHT) {
        float relativeMultiplyFactor = widthValue;
        return MathFunction::roundToInt(heightValueInPixel() * relativeMultiplyFactor);
    } else if (lengthType == LengthType::PIXEL) {
        return MathFunction::roundToInt(widthValue);
    }
    throw std::runtime_error("Unknown length type: " + std::to_string(lengthType));
}

template<class T> int Widget::heightLengthToPixel(float heightValue, LengthType lengthType, const T& widthValueInPixel) const {
    if (lengthType == LengthType::SCREEN_PERCENT) {
        return MathFunction::roundToInt(heightValue / 100.0f * (float) getSceneSize().Y);
    } else if (lengthType == LengthType::CONTAINER_PERCENT) {
        if (!getParentContainer()) {
            throw std::runtime_error("Missing parent container on the widget");
        }
        return MathFunction::roundToInt(heightValue / 100.0f * (float) ((Widget*) getParentContainer())->getHeight());
    } else if (lengthType == LengthType::PARENT_PERCENT) {
        if (!getParent()) {
            throw std::runtime_error("Missing parent on the widget");
        }
        return MathFunction::roundToInt(heightValue / 100.0f * (float) getParent()->getHeight());
    } else if (lengthType == LengthType::RATIO_TO_WIDTH) {
        float relativeMultiplyFactor = heightValue;
        return MathFunction::roundToInt(widthValueInPixel() * relativeMultiplyFactor);
    } else if (lengthType == LengthType::PIXEL) {
        return MathFunction::roundToInt(heightValue);
    }
    throw std::runtime_error("Unknown length type: " + std::to_string(lengthType));
}
