template<class T> std::shared_ptr<T> Widget::create(T* widget, Widget* parent) {
    std::shared_ptr<T> widgetPtr = std::shared_ptr<T>(widget);
    if(parent) {
        parent->addChild(widgetPtr);
    }
    return widgetPtr;
}