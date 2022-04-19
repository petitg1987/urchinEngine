#include <memory>
#include <utility>
#include <UrchinCommon.h>

#include <scene/ui/widget/window/Window.h>
#include <scene/InputDeviceKey.h>
#include <api/render/GenericRendererBuilder.h>

namespace urchin {

    Window::Window(Position position, Size size, std::string skinName, std::string titleKey) :
            Widget(position, size),
            skinName(std::move(skinName)),
            titleKey(std::move(titleKey)),
            mousePositionX(0),
            mousePositionY(0),
            state(DEFAULT),
            title(nullptr) {

    }

    std::shared_ptr<Window> Window::create(Widget* parent, Position position, Size size, std::string skinName, std::string titleKey) {
        return Widget::create<Window>(new Window(position, size, std::move(skinName), std::move(titleKey)), parent);
    }

    void Window::createOrUpdateWidget() {
        //detach children
        detachChild(title.get());

        //skin information
        auto windowChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "window", UdaAttribute("skin", skinName));

        auto skinChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute(), windowChunk);
        texWindow = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinChunk, &widgetOutline);

        if (!titleKey.empty()) {
            auto textSkinChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textSkin", UdaAttribute(), windowChunk);
            title = Text::create(this, Position(0.0f, 0.0f, LengthType::PIXEL), textSkinChunk->getStringValue(), i18n(titleKey));
            title->updatePosition(Position(0.0f, -((float)widgetOutline.topWidth + title->getHeight()) / 2.0f, LengthType::PIXEL));
        }

        //visual
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(getWidth(), 0.0f), Point2<float>(getWidth(), getHeight()),
                Point2<float>(0.0f, 0.0f), Point2<float>(getWidth(), getHeight()), Point2<float>(0.0f, getHeight())
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        windowRenderer = setupUiRenderer("window", ShapeType::TRIANGLE, false)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(texWindow, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy()))) //binding 3
                ->build();
    }

    WidgetType Window::getWidgetType() const {
        return WidgetType::WINDOW;
    }

    bool Window::onKeyPressEvent(unsigned int key) {
        bool propagateEvent = true;
        if (key == (int)InputDeviceKey::MOUSE_LEFT) {
            Rectangle2D titleZone(Point2<int>((int)getGlobalPositionX(), (int)getGlobalPositionY()),
                                  Point2<int>((int)getGlobalPositionX() + ((int) getWidth() - widgetOutline.rightWidth), (int)getGlobalPositionY() + widgetOutline.topWidth));
            Rectangle2D closeZone(Point2<int>((int)getGlobalPositionX() + ((int) getWidth() - widgetOutline.rightWidth), (int)getGlobalPositionY()),
                                  Point2<int>((int)getGlobalPositionX() + (int) getWidth(), (int)getGlobalPositionY() + widgetOutline.topWidth));

            if (!getUi3dData() && titleZone.collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
                mousePositionX = getMouseX() - MathFunction::roundToInt(getPositionX());
                mousePositionY = getMouseY() - MathFunction::roundToInt(getPositionY());
                state = MOVING;
            }

            if (closeZone.collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
                state = CLOSING;
            }

            if (widgetRectangle().collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
                notifyObservers(this, SET_IN_FOREGROUND);
                propagateEvent = false;
            }
        }

        return propagateEvent;
    }

    bool Window::onKeyReleaseEvent(unsigned int key) {
        Rectangle2D closeZone(Point2<int>((int)getGlobalPositionX() + ((int)getWidth() - widgetOutline.rightWidth), (int)getGlobalPositionY()),
                              Point2<int>((int)getGlobalPositionX() + (int)getWidth(), (int)getGlobalPositionY() + widgetOutline.topWidth));
        if (key == (int)InputDeviceKey::MOUSE_LEFT && state == CLOSING && closeZone.collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
            setIsVisible(false);
        }

        state = DEFAULT;

        return true;
    }

    bool Window::onMouseMoveEvent(int mouseX, int mouseY) {
        bool propagateEvent = true;
        if (state == MOVING) {
            auto positionPixelX = (float)(mouseX - mousePositionX);
            auto positionLengthX = widthPixelToLength(positionPixelX, getPosition().getXType());

            auto positionPixelY = (float)(mouseY - mousePositionY);
            auto positionLengthY = heightPixelToLength(positionPixelY, getPosition().getYType());

            updatePosition(Position(positionLengthX, getPosition().getXType(),
                                    positionLengthY, getPosition().getYType(),
                                    getPosition().getRelativeTo(), getPosition().getReferencePoint()));
            propagateEvent = false;
        } else if (widgetRectangle().collideWithPoint(Point2<int>(mouseX, mouseY))) {
            propagateEvent = false;
        }

        return propagateEvent;
    }

    void Window::onResetStateEvent() {
        state = DEFAULT;
    }

    void Window::prepareWidgetRendering(float, unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        updateProperties(windowRenderer.get(), projectionViewMatrix, Vector2<float>(getGlobalPositionX(), getGlobalPositionY()));
        windowRenderer->enableRenderer(renderingOrder);
    }

}
