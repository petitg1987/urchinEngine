#include <algorithm>

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/container/Container.h>
#include <scene/InputDeviceKey.h>

namespace urchin {

    Widget::Widget(Position position, Size size) :
            i18nService(nullptr),
            renderTarget(nullptr),
            shader(nullptr),
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
        detachChildren();
    }

    void Widget::initialize(RenderTarget& renderTarget, const Shader& shader, I18nService& i18nService) {
        this->sceneWidth = renderTarget.getWidth();
        this->sceneHeight = renderTarget.getHeight();

        this->renderTarget = &renderTarget;
        this->shader = &shader;
        this->i18nService = &i18nService;

        createOrUpdateWidget();
        for (auto& child : children) {
            child->initialize(renderTarget, shader, i18nService);
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
        assert(renderTarget);

        auto rendererBuilder = GenericRendererBuilder::create(name, *renderTarget, *shader, shapeType);

        //orthogonal matrix with origin at top left screen
        Matrix4<float> projectionMatrix(2.0f / (float)sceneWidth, 0.0f, -1.0f, 0.0f,
                              0.0f, 2.0f / (float)sceneHeight, -1.0f, 0.0f,
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f);
        rendererBuilder->addUniformData(sizeof(projectionMatrix), &projectionMatrix); //binding 0

        Vector2<int> translateVector(0, 0);
        rendererBuilder->addUniformData(sizeof(translateVector), &translateVector); //binding 1

        Container* parentContainer = retrieveParentContainer();
        if (parentContainer) {
            Vector2<int> scissorOffset = Vector2<int>(parentContainer->getPositionX(), parentContainer->getPositionY());
            Vector2<unsigned int> scissorSize = Vector2<unsigned int>(parentContainer->getWidth(), parentContainer->getHeight());
            rendererBuilder->enableScissor(scissorOffset, scissorSize);
        }

        return rendererBuilder;
    }

    void Widget::updateTranslateVector(GenericRenderer* renderer, const Vector2<int>& translateVector) const {
        renderer->updateUniformData(1, &translateVector);
    }

    RenderTarget& Widget::getRenderTarget() const {
        assert(renderTarget);
        return *renderTarget;
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

    void Widget::addChild(const std::shared_ptr<Widget>& childWidget) {
        if (childWidget->getParent()) {
            throw std::runtime_error("Cannot add a widget which already have a parent");
        }

        childWidget->parent = this;
        children.push_back(childWidget);

        if (renderTarget) {
            childWidget->initialize(getRenderTarget(), *shader, *i18nService);
        }
    }

    const std::vector<std::shared_ptr<Widget>>& Widget::getChildren() const {
        return children;
    }

    void Widget::detachChild(Widget* childWidget) {
        if (childWidget) {
            auto itFind = std::find_if(children.begin(), children.end(), [&childWidget](const auto &o) { return childWidget == o.get(); });
            if (itFind == children.end()) {
                throw std::runtime_error("The provided child widget is not a child of this widget");
            }

            childWidget->parent = nullptr;
            children.erase(itFind);
        }
    }

    void Widget::detachChildren() {
        for(auto it = children.begin(); it != children.end();) {
            (*it)->parent = nullptr;
            it = children.erase(it);
        }
    }

    void Widget::addEventListener(std::shared_ptr<EventListener> eventListener) {
        this->eventListeners.push_back(std::move(eventListener));
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
    * @return Relative position X of the widget
    */
    int Widget::getPositionX() const {
        if (position.getPositionTypeX() == LengthType::PERCENTAGE) {
            Container* parentContainer = retrieveParentContainer();
            int referenceWidth = parentContainer ? ((int)parentContainer->getWidth() - (parentContainer->getOutline().leftWidth + parentContainer->getOutline().rightWidth)) : (int)sceneWidth;
            return (int)(position.getPositionX() / 100.0f * (float)referenceWidth);
        }

        return (int)position.getPositionX();
    }

    /**
    * @return Relative position Y of the widget
    */
    int Widget::getPositionY() const {
        if (position.getPositionTypeY() == LengthType::PERCENTAGE) {
            Container* parentContainer = retrieveParentContainer();
            int referenceHeight = parentContainer ? ((int)parentContainer->getHeight() - (parentContainer->getOutline().topWidth + parentContainer->getOutline().bottomWidth)) : (int)sceneHeight;
            return (int)(position.getPositionY() / 100.0f * (float)referenceHeight);
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

        int startPosition = 0;
        if (position.getRelativeTo() == RelativeTo::PARENT_TOP_LEFT || position.getRelativeTo() == RelativeTo::PARENT_BOTTOM_LEFT) {
            startPosition = parent->getGlobalPositionX() + parent->getOutline().leftWidth;
        } else if (position.getRelativeTo() == RelativeTo::PARENT_TOP_RIGHT || position.getRelativeTo() == RelativeTo::PARENT_BOTTOM_RIGHT) {
            startPosition = parent->getGlobalPositionX() - parent->getOutline().rightWidth + (int)parent->getWidth();
        }

        return startPosition + getPositionX();
    }

    int Widget::getGlobalPositionY() const {
        if (!parent) {
            return getPositionY();
        }

        int startPosition = 0;
        if (position.getRelativeTo() == RelativeTo::PARENT_TOP_LEFT || position.getRelativeTo() == RelativeTo::PARENT_TOP_RIGHT) {
            startPosition = parent->getGlobalPositionY() + parent->getOutline().topWidth;
        } else if (position.getRelativeTo() == RelativeTo::PARENT_BOTTOM_LEFT || position.getRelativeTo() == RelativeTo::PARENT_BOTTOM_RIGHT) {
            startPosition = parent->getGlobalPositionY() - parent->getOutline().bottomWidth + (int)parent->getHeight();
        }

        return startPosition + getPositionY();
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
            Container* parentContainer = retrieveParentContainer();
            int referenceWidth = parentContainer ? ((int)parentContainer->getWidth() - (parentContainer->getOutline().leftWidth + parentContainer->getOutline().rightWidth)) : (int)sceneWidth;
            return (unsigned int)(size.getWidth() / 100.0f * (float)referenceWidth);
        } else if (size.getWidthSizeType() == LengthType::RELATIVE_LENGTH) {
            float relativeMultiplyFactor = size.getWidth();
            return (unsigned int)((float)getHeight() * relativeMultiplyFactor);
        }
        return (unsigned int)size.getWidth();
    }

    unsigned int Widget::getHeight() const {
        if (size.getHeightSizeType() == LengthType::PERCENTAGE) {
            Container* parentContainer = retrieveParentContainer();
            int referenceHeight = parentContainer ? ((int)parentContainer->getHeight() - (parentContainer->getOutline().topWidth + parentContainer->getOutline().bottomWidth)) : (int)sceneHeight;
            return (unsigned int)(size.getHeight() / 100.0f * (float)referenceHeight);
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

    Container* Widget::retrieveParentContainer() const {
        Widget *parent = getParent();
        while (parent != nullptr) {
            auto* containerParent = dynamic_cast<Container*>(parent);
            if (containerParent) {
                return containerParent;
            }
            parent = parent->getParent();
        }
        return nullptr;
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

            for (auto& child : children) {
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

            for (auto& child : children) {
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

            for (auto& child : children) {
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

            for (auto& child : children) {
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

            for (auto& child : children) {
                child->onResetState();
            }
        }
    }

    void Widget::handleWidgetReset() {
        if (widgetState == CLICKING) {
            widgetState = FOCUS;
            for (auto& eventListener : eventListeners) {
                eventListener->onMouseLeftClickRelease(this);
            }
        }

        if (widgetState == FOCUS) {
            widgetState = DEFAULT;
            for (auto& eventListener : eventListeners) {
                eventListener->onFocusLost(this);
            }
        }
    }

    void Widget::prepareRendering(float dt) {
        if (isVisible()) {
            prepareWidgetRendering(dt);

            for (auto& child : children) {
                child->prepareRendering(dt);
            }
        }
    }

}
