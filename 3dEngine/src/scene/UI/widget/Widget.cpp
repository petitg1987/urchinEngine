#include <algorithm>
#include <utility>

#include "scene/UI/widget/Widget.h"
#include "scene/InputDeviceKey.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {

    Widget::Widget(Widget* parent, Position position, Size size) :
            sceneWidth(parent ? parent->getSceneWidth() : 0),
            sceneHeight(parent ? parent->getSceneHeight() : 0),
            parent(parent),
            widgetState(Widget::DEFAULT),
            position(position),
            size(size),
            bIsVisible(true),
            mouseX(0),
            mouseY(0) {
        if (parent) {
            parent->children.emplace_back(this);
            initialize(parent->getRenderTarget(), parent->shader);
        }
    }

    Widget::~Widget() {
        std::vector<Widget*> childrenCopied(children);
        for (auto& child : childrenCopied) {
            delete child;
        }

        if (parent) {
            auto it = std::find(parent->children.begin(), parent->children.end(), this);
            parent->children.erase(it);
        }

        eventListeners.clear();
    }

    void Widget::initialize(std::shared_ptr<RenderTarget> renderTarget, std::shared_ptr<Shader> shader) {
        this->renderTarget = std::move(renderTarget);
        this->shader = std::move(shader);

        mProjectionShaderVar = ShaderVar(this->shader, "mProjection");
        translateDistanceShaderVar = ShaderVar(this->shader, "translateDistance");

        for (auto& child : children) {
            child->initialize(renderTarget, shader);
        }
    }

    void Widget::onResize(unsigned int sceneWidth, unsigned int sceneHeight, const Matrix3<float>& projectionMatrix) {
        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;
        this->projectionMatrix = projectionMatrix;

        createOrUpdateWidget();

        for (auto& child : children) {
            child->onResize(sceneWidth, sceneHeight, projectionMatrix);
        }
    }

    std::unique_ptr<GenericRendererBuilder> Widget::setupUiRenderer(ShapeType shapeType) const {
        assert(shader);
        auto rendererBuilder = std::make_unique<GenericRendererBuilder>(getRenderTarget(), shader, shapeType);
        rendererBuilder
                ->addShaderData(ShaderDataSender().sendData(mProjectionShaderVar, projectionMatrix)) //binding 0
                ->addShaderData(ShaderDataSender().sendData(translateDistanceShaderVar, Vector2<int>())); //binding 1
        return std::unique_ptr<GenericRendererBuilder>(rendererBuilder.release());
    }

    void Widget::updateTranslateVector(const std::unique_ptr<GenericRenderer>& renderer, const Vector2<int>& translateVector) {
        renderer->updateShaderData(1, ShaderDataSender().sendData(translateDistanceShaderVar, translateVector));
    }

    const std::shared_ptr<RenderTarget>& Widget::getRenderTarget() const {
        assert(renderTarget);
        return renderTarget;
    }

    unsigned int Widget::getSceneWidth() const {
        return sceneWidth;
    }

    unsigned int Widget::getSceneHeight() const {
        return sceneHeight;
    }

    const std::vector<Widget*>& Widget::getChildren() const {
        return children;
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
        if (position.getPositionTypeX() == LengthType::PERCENTAGE) {
            return (int)(position.getPositionX() / 100.0f * (float)sceneWidth);
        }

        return (int)position.getPositionX();
    }

    /**
    * @return Position Y of the widget relative to his parent
    */
    int Widget::getPositionY() const {
        if (position.getPositionTypeY() == LengthType::PERCENTAGE) {
            return (int)(position.getPositionY() / 100.0f * (float)sceneHeight);
        }

        return (int)position.getPositionY();
    }

    const WidgetOutline& Widget::getOutline() const {
        return widgetOutline;
    }

    int Widget::getGlobalPositionX() const {
        if (!parent) {
            return getPositionX();
        }

        return parent->getGlobalPositionX() + parent->getOutline().leftWidth + getPositionX();
    }

    int Widget::getGlobalPositionY() const {
        if (!parent) {
            return getPositionY();
        }

        return parent->getGlobalPositionY() + parent->getOutline().topWidth + getPositionY();
    }

    void Widget::setSize(Size size) {
        this->size = size;
    }

    Size Widget::getSize() const {
        return size;
    }

    unsigned int Widget::getWidth() const {
        if (size.getWidthSizeType() == LengthType::PERCENTAGE) {
            return (unsigned int)(size.getWidth() / 100.0f * (float)sceneWidth);
        }
        return (unsigned int)size.getWidth();
    }

    unsigned int Widget::getHeight() const {
        if (size.getHeightSizeType() == LengthType::PERCENTAGE) {
            return (unsigned int)(size.getHeight() / 100.0f * (float)sceneHeight);
        }
        return (unsigned int)size.getHeight();
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
            Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX() + (int)getWidth(), getGlobalPositionY() + (int)getHeight()));
            if (widgetRectangle.collideWithPoint(Point2<int>(mouseX, mouseY))) {
                widgetState = CLICKING;
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
            Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX() + (int)getWidth(), getGlobalPositionY() + (int)getHeight()));
            if (widgetRectangle.collideWithPoint(Point2<int>(mouseX, mouseY))) {
                if (widgetState == CLICKING) {
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
        Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX() + (int)getWidth(), getGlobalPositionY() + (int)getHeight()));
        if (widgetRectangle.collideWithPoint(Point2<int>(mouseX, mouseY))) {
            if (widgetState == DEFAULT) {
                widgetState = FOCUS;
                for (std::shared_ptr<EventListener>& eventListener : eventListeners) {
                    eventListener->onFocus(this);
                }
            }
        } else if (widgetState == FOCUS) {
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
        if (widgetState == CLICKING) {
            widgetState = FOCUS;
            for (std::shared_ptr<EventListener>& eventListener : eventListeners) {
                eventListener->onClickRelease(this);
            }
        }

        if (widgetState == FOCUS) {
            widgetState = DEFAULT;
            for (std::shared_ptr<EventListener>& eventListener : eventListeners) {
                eventListener->onFocusLost(this);
            }
        }
    }

    void Widget::display(float dt) {
        displayWidget(dt);

        for (auto& child : children) {
            if (child->isVisible()) { //TODO disable widget for render target ?
                child->display(dt);
            }
        }
    }

}
