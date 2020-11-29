#include <algorithm>

#include "scene/GUI/widget/Widget.h"
#include "scene/InputDeviceKey.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {

    Widget::Widget(Position position, Size size) :
        sceneWidth(0),
        sceneHeight(0),
        parent(nullptr),
        widgetState(Widget::DEFAULT),
        position(position),
        size(size),
        bIsVisible(true),
        mouseX(0),
        mouseY(0) {

    }

    Widget::~Widget() {
        for (auto& child : children) {
            delete child;
        }

        eventListeners.clear();
    }

    void Widget::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;

        createOrUpdateWidget();

        for (auto& child : children) {
            child->onResize(sceneWidth, sceneHeight);
        }
    }

    unsigned int Widget::getSceneWidth() const {
        return sceneWidth;
    }

    unsigned int Widget::getSceneHeight() const {
        return sceneHeight;
    }

    void Widget::addChild(Widget* child) {
        child->setParent(this);
        children.push_back(child);
    }

    void Widget::removeChild(Widget* child) {
        if (child) {
            auto it = std::find(children.begin(), children.end(), child);
            delete child;
            children.erase(it);
        }
    }

    const std::vector<Widget *> &Widget::getChildren() const {
        return children;
    }

    void Widget::setParent(Widget* parent) {
        this->parent = parent;
    }

    Widget* Widget::getParent() const {
        return parent;
    }

    void Widget::addEventListener(const std::shared_ptr<EventListener>& eventListener) {
        this->eventListeners.push_back(eventListener);
    }

    const std::vector<std::shared_ptr<EventListener>>& Widget::getEventListeners() const {
        return eventListeners;
    }

    Widget::WidgetStates Widget::getWidgetState() const {
        return widgetState;
    }

    void Widget::setPosition(Position position) {
        this->position = position;
    }

    Position Widget::getPosition() const {
        return position;
    }

    /**
    * @return Position X of the widget relative to his parent
    */
    int Widget::getPositionX() const {
        if (position.getPositionTypeX()==Position::PERCENTAGE) {
            return static_cast<int>(position.getPositionX() * (float)sceneWidth);
        }

        return static_cast<int>(position.getPositionX());
    }

    /**
    * @return Position Y of the widget relative to his parent
    */
    int Widget::getPositionY() const {
        if (position.getPositionTypeY()==Position::PERCENTAGE) {
            return static_cast<int>(position.getPositionY() * (float)sceneHeight);
        }

        return static_cast<int>(position.getPositionY());
    }

    int Widget::getGlobalPositionX() const {
        if (!parent) {
            return getPositionX();
        }

        return parent->getGlobalPositionX() + getPositionX();
    }

    int Widget::getGlobalPositionY() const {
        if (!parent) {
            return getPositionY();
        }

        return parent->getGlobalPositionY() + getPositionY();
    }

    void Widget::setSize(Size size) {
        this->size = size;
    }

    Size Widget::getSize() const {
        return size;
    }

    unsigned int Widget::getWidth() const {
        if (size.getWidthSizeType()==Size::PERCENTAGE) {
            return static_cast<unsigned int>(size.getWidth() * (float)sceneWidth);
        }
        return static_cast<unsigned int>(size.getWidth());
    }

    unsigned int Widget::getHeight() const {
        if (size.getHeightSizeType()==Size::PERCENTAGE) {
            return static_cast<unsigned int>(size.getHeight() * (float)sceneHeight);
        }
        return static_cast<unsigned int>(size.getHeight());
    }

    void Widget::setIsVisible(bool isVisible) {
        this->bIsVisible = isVisible;
    }

    bool Widget::isVisible() const {
        return bIsVisible;
    }

    bool Widget::onKeyPress(unsigned int key) {
        handleWidgetKeyDown(key);

        bool propagateEvent = onKeyPressEvent(key);

        for (auto& child : children) {
            if (child->isVisible() && !child->onKeyPress(key)) {
                return false;
            }
        }

        return propagateEvent;
    }

    bool Widget::onKeyPressEvent(unsigned int) {
        return true;
    }

    void Widget::handleWidgetKeyDown(unsigned int key) {
        if (key == InputDeviceKey::MOUSE_LEFT) {
            Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX()+getWidth(), getGlobalPositionY()+getHeight()));
            if (widgetRectangle.collideWithPoint(Point2<int>(mouseX, mouseY))) {
                widgetState=CLICKING;
                for (std::shared_ptr<EventListener>& eventListener : eventListeners) {
                    eventListener->onClick(this);
                }
            }
        }
    }

    bool Widget::onKeyRelease(unsigned int key) {
        handleWidgetKeyUp(key);

        bool propagateEvent = onKeyReleaseEvent(key);

        for (auto& child : children) {
            if (child->isVisible() && !child->onKeyRelease(key)) {
                return false;
            }
        }
        return propagateEvent;
    }

    bool Widget::onKeyReleaseEvent(unsigned int) {
        return true;
    }

    void Widget::handleWidgetKeyUp(unsigned int key) {
        if (key == InputDeviceKey::MOUSE_LEFT) {
            Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX()+getWidth(), getGlobalPositionY()+getHeight()));
            if (widgetRectangle.collideWithPoint(Point2<int>(mouseX, mouseY))) {
                if (widgetState==CLICKING) {
                    widgetState = FOCUS;
                    for (std::shared_ptr<EventListener>& eventListener : eventListeners) {
                        eventListener->onClickRelease(this);
                    }
                } else {
                    widgetState = FOCUS;
                }
            } else {
                widgetState = DEFAULT;
            }
        }
    }

    bool Widget::onChar(unsigned int character) {
        if (!onCharEvent(character)) {
            return false;
        }

        for (auto& child : children) {
            if (child->isVisible() && !child->onChar(character)) {
                return false;
            }
        }
        return true;
    }

    bool Widget::onCharEvent(unsigned int) {
        return true;
    }

    bool Widget::onMouseMove(int mouseX, int mouseY) {
        this->mouseX = mouseX;
        this->mouseY = mouseY;

        handleWidgetMouseMove(mouseX, mouseY);

        bool propagateEvent = onMouseMoveEvent(mouseX, mouseY);

        for (auto& child : children) {
            if (child->isVisible()) {
                if (!child->onMouseMove(mouseX, mouseY)) {
                    return false;
                }
            }
        }
        return propagateEvent;
    }

    bool Widget::onMouseMoveEvent(int, int) {
        return true;
    }

    void Widget::handleWidgetMouseMove(int mouseX, int mouseY) {
        Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX()+getWidth(), getGlobalPositionY()+getHeight()));
        if (widgetRectangle.collideWithPoint(Point2<int>(mouseX, mouseY))) {
            if (widgetState==DEFAULT) {
                widgetState = FOCUS;
                for (std::shared_ptr<EventListener>& eventListener : eventListeners) {
                    eventListener->onFocus(this);
                }
            }
        } else if (widgetState==FOCUS) {
            widgetState = DEFAULT;
            for (std::shared_ptr<EventListener>& eventListener : eventListeners) {
                eventListener->onFocusLost(this);
            }
        }
    }

    int Widget::getMouseX() const {
        return mouseX;
    }

    int Widget::getMouseY() const {
        return mouseY;
    }

    void Widget::reset() {
        for (auto& child : children) {
            if (child->isVisible()) {
                child->reset();
            }
        }
    }

    void Widget::onDisable() {
        handleDisable();

        for (auto& child : children) {
            if (child->isVisible()) {
                child->onDisable();
            }
        }
    }

    void Widget::handleDisable() {
        if (widgetState==CLICKING) {
            widgetState = FOCUS;
            for (std::shared_ptr<EventListener>& eventListener : eventListeners) {
                eventListener->onClickRelease(this);
            }
        }

        if (widgetState==FOCUS) {
            widgetState = DEFAULT;
            for (std::shared_ptr<EventListener>& eventListener : eventListeners) {
                eventListener->onFocusLost(this);
            }
        }
    }

    void Widget::display(const RenderTarget* renderTarget, const ShaderVar& translateDistanceShaderVar, float dt) {
        for (auto& child : children) {
            if (child->isVisible()) {
                Vector2<int> translateVector(child->getGlobalPositionX(), child->getGlobalPositionY());
                ShaderDataSender().sendData(translateDistanceShaderVar, translateVector);

                child->display(renderTarget, translateDistanceShaderVar, dt);
            }
        }
    }

}
