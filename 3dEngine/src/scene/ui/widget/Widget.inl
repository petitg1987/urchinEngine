template<class T> std::shared_ptr<T> Widget::create(T* widget, Widget* parent) {
    std::shared_ptr<T> widgetPtr = std::shared_ptr<T>(widget);
    if (parent) {
        parent->addChild(widgetPtr);
    }
    return widgetPtr;
}

template<class T> float Widget::widthLengthToPixel(float widthValue, LengthType lengthType, const T& heightValueInPixel) const {
    if (lengthType == SCREEN_PERCENT) {
        return widthValue / 100.0f * (float)getSceneSize().X;
    } else if (lengthType == CONTAINER_PERCENT) {
        const auto* parentContainer = (Widget*)getParentContainer();
        if (!parentContainer) {
            throw std::runtime_error("Missing parent container on the widget");
        }
        return widthValue / 100.0f * (parentContainer->getWidth() - (float)parentContainer->getOutline().leftWidth - (float)parentContainer->getOutline().rightWidth);
    } else if (lengthType == PARENT_PERCENT) {
        if (!getParent()) {
            throw std::runtime_error("Missing parent on the widget");
        }
        return widthValue / 100.0f * (getParent()->getWidth() - (float)getParent()->getOutline().leftWidth - (float)getParent()->getOutline().rightWidth);
    } else if (lengthType == RATIO_TO_HEIGHT) {
        float relativeMultiplyFactor = widthValue;
        return heightValueInPixel() * relativeMultiplyFactor;
    } else if (lengthType == PIXEL) {
        return widthValue;
    }
    throw std::runtime_error("Unknown length type: " + std::to_string(lengthType));
}

template<class T> float Widget::heightLengthToPixel(float heightValue, LengthType lengthType, const T& widthValueInPixel) const {
    if (lengthType == SCREEN_PERCENT) {
        return heightValue / 100.0f * (float) getSceneSize().Y;
    } else if (lengthType == CONTAINER_PERCENT) {
        const auto* parentContainer = (Widget*)getParentContainer();
        if (!parentContainer) {
            throw std::runtime_error("Missing parent container on the widget");
        }
        return heightValue / 100.0f * (parentContainer->getHeight() - (float)parentContainer->getOutline().topWidth - (float)parentContainer->getOutline().bottomWidth);
    } else if (lengthType == PARENT_PERCENT) {
        if (!getParent()) {
            throw std::runtime_error("Missing parent on the widget");
        }
        return heightValue / 100.0f * (getParent()->getHeight() - (float)getParent()->getOutline().topWidth - (float)getParent()->getOutline().bottomWidth);
    } else if (lengthType == RATIO_TO_WIDTH) {
        float relativeMultiplyFactor = heightValue;
        return widthValueInPixel() * relativeMultiplyFactor;
    } else if (lengthType == PIXEL) {
        return heightValue;
    }
    throw std::runtime_error("Unknown length type: " + std::to_string(lengthType));
}
