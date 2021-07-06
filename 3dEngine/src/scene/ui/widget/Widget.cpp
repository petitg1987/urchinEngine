#include <algorithm>
#include <utility>

#include <scene/ui/widget/Widget.h>
#include <scene/InputDeviceKey.h>

namespace urchin {

    Widget::Widget(Widget* parent, Position position, Size size) :
            i18nService(nullptr),
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
            initialize(parent->getRenderTarget(), parent->shader, parent->i18nService, false /*cannot call virtual method in constructor*/);
        }
    }

    Widget::~Widget() {
        deleteChildren();

        if (parent) {
            auto it = std::find(parent->children.begin(), parent->children.end(), this);
            parent->children.erase(it);
        }

        eventListeners.clear();
    }

    void Widget::initialize(const std::shared_ptr<RenderTarget>& renderTarget, const std::shared_ptr<Shader>& shader, I18nService* i18nService, bool createWidget) {
        this->sceneWidth = renderTarget->getWidth();
        this->sceneHeight = renderTarget->getHeight();

        this->renderTarget = renderTarget;
        this->shader = shader;
        this->i18nService = i18nService;

        if(createWidget) {
            createOrUpdateWidget();
        }

        for (auto& child : children) {
            child->initialize(renderTarget, shader, i18nService, createWidget);
        }
    }

    void Widget::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;
        createOrUpdateWidget();

        for (auto& child : children) {
            child->onResize(sceneWidth, sceneHeight);
        }
    }

    std::shared_ptr<GenericRendererBuilder> Widget::setupUiRenderer(const std::string& name, ShapeType shapeType) const {
        assert(shader);

        //orthogonal matrix with origin at top left screen
        Matrix4<float> projectionMatrix(2.0f / (float)sceneWidth, 0.0f, -1.0f, 0.0f,
                              0.0f, 2.0f / (float)sceneHeight, -1.0f, 0.0f,
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f);

        Vector2<int> translateVector(0, 0);

        return GenericRendererBuilder::create(name, getRenderTarget(), shader, shapeType)
                ->addUniformData(sizeof(projectionMatrix), &projectionMatrix) //binding 0
                ->addUniformData(sizeof(translateVector), &translateVector); //binding 1
    }

    void Widget::updateTranslateVector(const std::shared_ptr<GenericRenderer>& renderer, const Vector2<int>& translateVector) const {
        renderer->updateUniformData(1, &translateVector);
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

    Widget* Widget::getParent() const {
        return parent;
    }

    const std::vector<Widget*>& Widget::getChildren() const {
        return children;
    }

    void Widget::deleteChildren() {
        std::vector<Widget*> childrenCopied(children);
        for (auto& child : childrenCopied) {
            delete child;
        }
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

    void Widget::updatePosition(Position position) {
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

    void Widget::updateSize(Size size) {
        setSize(size);
        createOrUpdateWidget();
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
        } else if (size.getWidthSizeType() == LengthType::RELATIVE_LENGTH) {
            float relativeMultiplyFactor = size.getWidth();
            return (unsigned int)((float)getHeight() * relativeMultiplyFactor);
        }
        return (unsigned int)size.getWidth();
    }

    unsigned int Widget::getHeight() const {
        if (size.getHeightSizeType() == LengthType::PERCENTAGE) {
            return (unsigned int)(size.getHeight() / 100.0f * (float)sceneHeight);
        } else if (size.getHeightSizeType() == LengthType::RELATIVE_LENGTH) {
            float relativeMultiplyFactor = size.getHeight();
            return (unsigned int)((float)getWidth() * relativeMultiplyFactor);
        }
        return (unsigned int)size.getHeight();
    }

    void Widget::setIsVisible(bool isVisible) {
        if (!isVisible) {
            onResetState();
        }
        this->bIsVisible = isVisible;
    }

    bool Widget::isVisible() const {
        return bIsVisible;
    }

    bool Widget::onKeyPress(unsigned int key) {
        bool propagateEvent = true;
        if (isVisible()) {
            bool widgetStateUpdated = handleWidgetKeyDown(key);
            propagateEvent = onKeyPressEvent(key);

            if (widgetStateUpdated && widgetState == Widget::CLICKING) {
                for (auto& eventListener : eventListeners) {
                    eventListener->onMouseLeftClick(this);
                }
            }

            for (auto &child : children) {
                if (!child->onKeyPress(key)) {
                    return false;
                }
            }
        }
        return propagateEvent;
    }

    bool Widget::onKeyPressEvent(unsigned int) {
        return true;
    }

    bool Widget::handleWidgetKeyDown(unsigned int key) {
        bool widgetStateUpdated = false;
        if (key == InputDeviceKey::MOUSE_LEFT) {
            Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX() + (int)getWidth(), getGlobalPositionY() + (int)getHeight()));
            if (widgetRectangle.collideWithPoint(Point2<int>(mouseX, mouseY))) {
                widgetState = CLICKING;
                widgetStateUpdated = true;
            }
        }
        return widgetStateUpdated;
    }

    bool Widget::onKeyRelease(unsigned int key) {
        bool propagateEvent = true;
        if (isVisible()) {
            bool widgetStateUpdated = handleWidgetKeyUp(key);
            propagateEvent = onKeyReleaseEvent(key);

            if (widgetStateUpdated && widgetState == Widget::FOCUS) {
                for (auto& eventListener : eventListeners) {
                    eventListener->onMouseLeftClickRelease(this);
                }
            }

            for (auto &child : children) {
                if (!child->onKeyRelease(key)) {
                    return false;
                }
            }
        }
        return propagateEvent;
    }

    bool Widget::onKeyReleaseEvent(unsigned int) {
        return true;
    }

    bool Widget::handleWidgetKeyUp(unsigned int key) {
        bool widgetStateUpdated = false;
        if (key == InputDeviceKey::MOUSE_LEFT) {
            Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX() + (int)getWidth(), getGlobalPositionY() + (int)getHeight()));
            if (widgetRectangle.collideWithPoint(Point2<int>(mouseX, mouseY))) {
                if (widgetState == CLICKING) {
                    widgetState = FOCUS;
                    widgetStateUpdated = true;
                }
            } else {
                widgetState = DEFAULT;
                widgetStateUpdated = true;
            }
        }
        return widgetStateUpdated;
    }

    bool Widget::onChar(char32_t unicodeCharacter) {
        bool propagateEvent = true;
        if (isVisible()) {
            propagateEvent = onCharEvent(unicodeCharacter);

            for (auto &child : children) {
                if (!child->onChar(unicodeCharacter)) {
                    return false;
                }
            }
        }
        return propagateEvent;
    }

    bool Widget::onCharEvent(char32_t) {
        return true;
    }

    bool Widget::onMouseMove(int mouseX, int mouseY) {
        this->mouseX = mouseX;
        this->mouseY = mouseY;

        bool propagateEvent = true;
        if (isVisible()) {
            bool widgetStateUpdated = handleWidgetMouseMove(mouseX, mouseY);
            propagateEvent = onMouseMoveEvent(mouseX, mouseY);

            if (widgetStateUpdated && widgetState == Widget::FOCUS) {
                for (auto& eventListener : eventListeners) {
                    eventListener->onFocus(this);
                }
            } else if (widgetStateUpdated && widgetState == Widget::DEFAULT) {
                for (auto& eventListener : eventListeners) {
                    eventListener->onFocusLost(this);
                }
            }

            for (auto &child : children) {
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

    bool Widget::handleWidgetMouseMove(int mouseX, int mouseY) {
        bool widgetStateUpdated = false;
        Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX() + (int)getWidth(), getGlobalPositionY() + (int)getHeight()));
        if (widgetRectangle.collideWithPoint(Point2<int>(mouseX, mouseY))) {
            if (widgetState == DEFAULT) {
                widgetState = FOCUS;
                widgetStateUpdated = true;
            }
        } else if (widgetState == FOCUS) {
            widgetState = DEFAULT;
            widgetStateUpdated = true;
        }
        return widgetStateUpdated;
    }

    int Widget::getMouseX() const {
        return mouseX;
    }

    int Widget::getMouseY() const {
        return mouseY;
    }

    void Widget::onResetState() {
        if (isVisible()) {
            handleWidgetReset();

            for (auto &child : children) {
                child->onResetState();
            }
        }
    }

    void Widget::handleWidgetReset() {
        if (widgetState == CLICKING) {
            widgetState = FOCUS;
            for (std::shared_ptr<EventListener>& eventListener : eventListeners) {
                eventListener->onMouseLeftClickRelease(this);
            }
        }

        if (widgetState == FOCUS) {
            widgetState = DEFAULT;
            for (std::shared_ptr<EventListener>& eventListener : eventListeners) {
                eventListener->onFocusLost(this);
            }
        }
    }

    void Widget::prepareRendering(float dt) {
        if (isVisible()) {
            prepareWidgetRendering(dt);

            for (auto &child : children) {
                child->prepareRendering(dt);
            }
        }
    }

}
